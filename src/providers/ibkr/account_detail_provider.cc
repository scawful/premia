#include "premia/providers/ibkr/account_detail_provider.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "DefaultEWrapper.h"
#include "EClientSocket.h"
#include "EReader.h"
#include "EReaderOSSignal.h"

namespace premia::providers::ibkr {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

constexpr int kSummaryRequestId = 9001;

auto MakeMoney(double value) -> domain::Money {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return domain::Money{oss.str(), "USD"};
}

auto FormatDouble(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

struct Config {
  std::string host;
  int port = 0;
  int client_id = 0;
  std::string account_id;
};

auto ReadConfig(const std::string& path) -> Config {
  std::ifstream file(path);
  if (!file.good()) {
    throw std::runtime_error("ibkr config unavailable");
  }

  pt::ptree tree;
  pt::read_json(file, tree);
  Config config;
  config.host = tree.get<std::string>("host", "");
  config.port = tree.get<int>("port", 0);
  config.client_id = tree.get<int>("client_id", 0);
  config.account_id = tree.get<std::string>("account_id", "");
  if (config.host.empty() || config.port <= 0) {
    throw std::runtime_error("ibkr config is incomplete");
  }
  return config;
}

class SnapshotClient : public DefaultEWrapper {
 public:
  SnapshotClient()
      : signal_(200),
        client_(&*this, &signal_) {}

  auto Connect(const Config& config) -> bool {
    if (!client_.eConnect(config.host.c_str(), config.port, config.client_id)) {
      error_text_ = "unable to connect to TWS/IB Gateway";
      return false;
    }
    reader_ = std::make_unique<EReader>(&client_, &signal_);
    reader_->start();
    return true;
  }

  auto Fetch(const Config& config) -> application::AccountDetail {
    if (!Connect(config)) {
      throw std::runtime_error(error_text_);
    }

    WaitFor([this]() {
      return next_valid_id_ready_ || managed_accounts_ready_ || fatal_error_;
    }, 5);
    if (fatal_error_) {
      throw std::runtime_error(error_text_);
    }
    if (!next_valid_id_ready_ && !managed_accounts_ready_) {
      client_.eDisconnect();
      throw std::runtime_error(
          "ibkr handshake timed out; accept or authorize the API session in "
          "TWS/IB Gateway");
    }
    if (!managed_accounts_ready_) {
      client_.reqManagedAccts();
      WaitFor([this]() { return managed_accounts_ready_ || fatal_error_; }, 5);
    }
    if (fatal_error_) {
      throw std::runtime_error(error_text_);
    }

    if (managed_accounts_.empty()) {
      throw std::runtime_error(
          "ibkr connected but returned no managed accounts; confirm the API "
          "session is accepted in TWS/IB Gateway");
    }

    target_account_ = config.account_id.empty() ? managed_accounts_.front() : config.account_id;
    client_.reqAccountSummary(kSummaryRequestId, "All",
                              "NetLiquidation,TotalCashValue,AvailableFunds,BuyingPower,EquityWithLoanValue,GrossPositionValue,MaintMarginReq");
    client_.reqAccountUpdates(true, target_account_);

    WaitFor([this]() { return account_summary_done_ && account_download_done_; }, 8);
    client_.cancelAccountSummary(kSummaryRequestId);
    client_.reqAccountUpdates(false, target_account_);
    disconnecting_ = true;
    client_.eDisconnect();

    if (fatal_error_) {
      throw std::runtime_error(error_text_);
    }
    if (!(account_summary_done_ && account_download_done_)) {
      throw std::runtime_error("ibkr snapshot timed out");
    }

    application::AccountDetail detail;
    detail.account_id = target_account_;
    detail.cash = MakeMoney(summary_value("TotalCashValue"));
    detail.net_liquidation = MakeMoney(summary_value("NetLiquidation"));
    detail.available_funds = MakeMoney(summary_value("AvailableFunds"));
    detail.long_market_value = MakeMoney(summary_value("GrossPositionValue"));
    detail.short_market_value = MakeMoney(0.0);
    detail.buying_power = MakeMoney(summary_value("BuyingPower"));
    detail.equity = MakeMoney(summary_value("EquityWithLoanValue"));
    detail.equity_percentage = "100.00";
    detail.margin_balance = MakeMoney(summary_value("MaintMarginReq"));
    detail.positions = positions_;
    return detail;
  }

  void error(int id, int errorCode, const std::string& errorString) override {
    (void)id;
    if (errorCode == 2104 || errorCode == 2106 || errorCode == 2158 ||
        errorCode == 2108 || errorCode == 2107) {
      return;
    }
    if (disconnecting_ && errorCode == 509) {
      return;
    }
    fatal_error_ = true;
    std::ostringstream oss;
    oss << "ibkr error " << errorCode << ": " << errorString;
    error_text_ = oss.str();
  }

  void managedAccounts(const std::string& accountsList) override {
    std::stringstream ss(accountsList);
    std::string item;
    while (std::getline(ss, item, ',')) {
      if (!item.empty()) {
        managed_accounts_.push_back(item);
      }
    }
    managed_accounts_ready_ = true;
  }

  void nextValidId(OrderId orderId) override {
    next_valid_id_ = orderId;
    next_valid_id_ready_ = true;
  }

  void accountSummary(int reqId, const std::string& account,
                      const std::string& tag, const std::string& value,
                      const std::string& currency) override {
    (void)reqId;
    (void)currency;
    if (account == target_account_) {
      summary_[tag] = value;
    }
  }

  void accountSummaryEnd(int reqId) override {
    if (reqId == kSummaryRequestId) {
      account_summary_done_ = true;
    }
  }

  void updatePortfolio(const Contract& contract, double position,
                       double marketPrice, double marketValue,
                       double averageCost, double unrealizedPNL,
                       double realizedPNL,
                       const std::string& accountName) override {
    (void)marketPrice;
    (void)realizedPNL;
    if (accountName != target_account_) {
      return;
    }
    if (position == 0.0) {
      return;
    }
    application::AccountPositionRow row;
    row.symbol = contract.symbol;
    row.name = contract.symbol;
    row.day_profit_loss = MakeMoney(0.0);
    row.day_profit_loss_percent = "0.00";
    row.average_price = MakeMoney(averageCost);
    row.market_value = MakeMoney(marketValue);
    row.quantity = FormatDouble(position);
    positions_.push_back(row);
  }

  void accountDownloadEnd(const std::string& accountName) override {
    if (accountName == target_account_) {
      account_download_done_ = true;
    }
  }

 private:
  template <typename Predicate>
  void WaitFor(Predicate predicate, int seconds) {
    const auto deadline = std::chrono::steady_clock::now() +
                          std::chrono::seconds(seconds);
    while (!predicate() && std::chrono::steady_clock::now() < deadline) {
      signal_.waitForSignal();
      if (reader_) {
        reader_->processMsgs();
      }
    }
  }

  auto summary_value(const std::string& tag) const -> double {
    auto it = summary_.find(tag);
    if (it == summary_.end()) {
      return 0.0;
    }
    try {
      return std::stod(it->second);
    } catch (...) {
      return 0.0;
    }
  }

  EReaderOSSignal signal_;
  EClientSocket client_;
  std::unique_ptr<EReader> reader_;
  bool managed_accounts_ready_ = false;
  bool next_valid_id_ready_ = false;
  bool account_summary_done_ = false;
  bool account_download_done_ = false;
  bool disconnecting_ = false;
  bool fatal_error_ = false;
  std::string error_text_;
  std::string target_account_;
  std::optional<OrderId> next_valid_id_;
  std::vector<std::string> managed_accounts_;
  std::map<std::string, std::string> summary_;
  std::vector<application::AccountPositionRow> positions_;
};

}  // namespace

AccountDetailProvider::AccountDetailProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto AccountDetailProvider::GetAccountDetail() const -> application::AccountDetail {
  auto config = ReadConfig(config_path_);
  SnapshotClient client;
  return client.Fetch(config);
}

}  // namespace premia::providers::ibkr

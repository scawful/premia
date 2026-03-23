#include "premia/providers/schwab/account_detail_provider.hpp"

#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Schwab/client.h"

namespace premia::providers::schwab {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount.empty() ? "0.00" : amount, "USD"};
}

auto FormatDouble(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

auto IsPlaceholderValue(const std::string& value) -> bool {
  return value.empty() || value.rfind("YOUR_", 0) == 0;
}

auto ReadConfigTree(const std::string& path, pt::ptree& tree) -> bool {
  std::ifstream file(path);
  if (!file.good()) {
    return false;
  }
  try {
    pt::read_json(file, tree);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

auto HasUsableConfig(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(path, tree)) {
    return false;
  }
  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto LoadAuthorizedClient(::premia::schwab::Client& client,
                          const std::string& config_path,
                          const std::string& token_path) -> bool {
  if (!HasUsableConfig(config_path)) {
    return false;
  }
  if (!client.LoadConfig(config_path)) {
    return false;
  }
  client.LoadTokens(token_path);
  if (client.HasValidAccessToken()) {
    return true;
  }
  if (client.HasValidRefreshToken() && client.RefreshAccessToken()) {
    client.SaveTokens(token_path);
    return client.HasValidAccessToken();
  }
  return false;
}

auto ParseAccount(const std::string& response) -> application::AccountDetail {
  if (response.empty()) {
    throw std::runtime_error("empty schwab account response");
  }

  std::istringstream ss(response);
  pt::ptree root;
  pt::read_json(ss, root);

  const auto& account = root.get_child("securitiesAccount");
  const auto& balances = account.get_child("currentBalances");

  application::AccountDetail detail;
  detail.account_id = account.get<std::string>("accountNumber", "");
  detail.cash = MakeMoney(FormatDouble(balances.get<double>("cashBalance", 0.0)));
  detail.net_liquidation = MakeMoney(FormatDouble(balances.get<double>("liquidationValue", 0.0)));
  detail.available_funds = MakeMoney(FormatDouble(balances.get<double>("availableFunds", 0.0)));
  detail.long_market_value = MakeMoney(FormatDouble(balances.get<double>("longMarketValue", 0.0)));
  detail.short_market_value = MakeMoney(FormatDouble(balances.get<double>("shortMarketValue", 0.0)));
  detail.buying_power = MakeMoney(FormatDouble(balances.get<double>("buyingPower", 0.0)));
  detail.equity = MakeMoney(FormatDouble(balances.get<double>("equity", 0.0)));
  detail.equity_percentage = FormatDouble(balances.get<double>("equityPercentage", 0.0));
  detail.margin_balance = MakeMoney(FormatDouble(balances.get<double>("marginBalance", 0.0)));

  if (auto positions = account.get_child_optional("positions")) {
    for (const auto& item : positions.get()) {
      const auto& position = item.second;
      const auto& instrument = position.get_child("instrument");
      const auto symbol = instrument.get<std::string>("symbol", "UNKNOWN");
      detail.positions.push_back({
          symbol,
          symbol,
          MakeMoney(FormatDouble(position.get<double>("currentDayProfitLoss", 0.0))),
          FormatDouble(position.get<double>("currentDayProfitLossPercentage", 0.0)),
          MakeMoney(FormatDouble(position.get<double>("averagePrice", position.get<double>("averageLongPrice", 0.0)))),
          MakeMoney(FormatDouble(position.get<double>("marketValue", 0.0))),
          FormatDouble(position.get<double>("longQuantity", 0.0)),
      });
    }
  }

  return detail;
}

auto PickBestAccountResponse(const std::vector<std::string>& responses) -> std::string {
  double best_score = -std::numeric_limits<double>::infinity();
  std::string best_response;
  for (const auto& response : responses) {
    if (response.empty()) {
      continue;
    }
    std::istringstream ss(response);
    pt::ptree root;
    pt::read_json(ss, root);
    const auto& account = root.get_child("securitiesAccount");
    const auto& balances = account.get_child("currentBalances");
    const auto liquidation = balances.get<double>("liquidationValue", 0.0);
    const auto positions = account.get_child_optional("positions");
    const auto position_count = positions ? static_cast<double>(positions->size()) : 0.0;
    const auto score = liquidation + (position_count > 0 ? 1000000.0 : 0.0);
    if (score > best_score) {
      best_score = score;
      best_response = response;
    }
  }
  if (best_response.empty()) {
    throw std::runtime_error("no usable schwab account responses");
  }
  return best_response;
}

}  // namespace

AccountDetailProvider::AccountDetailProvider(std::string config_path,
                                             std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto AccountDetailProvider::GetAccountDetail() const -> application::AccountDetail {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }
  if (!client.GetAccountNumbers()) {
    throw std::runtime_error("schwab account bootstrap unavailable");
  }

  std::vector<std::string> responses;
  for (const auto& account : client.GetAllAccountHashes()) {
    const auto response = client.GetAccount(account.hash_value);
    if (!response.empty()) {
      responses.push_back(response);
    }
  }
  return ParseAccount(PickBestAccountResponse(responses));
}

}  // namespace premia::providers::schwab

#include "premia/providers/tda/account_detail_provider.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TDAmeritrade/client.h"
#include "TDAmeritrade/data/Account.hpp"
#include "TDAmeritrade/parser.h"

namespace premia::providers::tda {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount.empty() ? "0.00" : amount, "USD"};
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

auto LoadAccount(const std::string& config_path) -> ::premia::tda::Account {
  pt::ptree tree;
  if (!ReadConfigTree(config_path, tree)) {
    throw std::runtime_error("tda account detail config unavailable");
  }

  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  if (IsPlaceholderValue(consumer_key) || IsPlaceholderValue(refresh_token)) {
    throw std::runtime_error("tda account detail config is placeholder");
  }

  ::premia::tda::Client client;
  ::premia::tda::Parser parser;
  client.addAuth(consumer_key, refresh_token);
  client.fetch_access_token();
  (void)client.get_all_accounts();
  const auto account_ids = client.get_all_account_ids();
  if (account_ids.empty()) {
    throw std::runtime_error("tda returned no accounts");
  }
  return parser.parse_account(parser.read_response(client.get_account(account_ids.front())));
}

}  // namespace

AccountDetailProvider::AccountDetailProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto AccountDetailProvider::GetAccountDetail() const -> application::AccountDetail {
  auto account = LoadAccount(config_path_);
  application::AccountDetail detail;
  detail.account_id = account.get_account_variable("accountId");
  detail.cash = MakeMoney(account.get_balance_variable("cashBalance"));
  detail.net_liquidation = MakeMoney(account.get_balance_variable("liquidationValue"));
  detail.available_funds = MakeMoney(account.get_balance_variable("availableFunds"));
  detail.long_market_value = MakeMoney(account.get_balance_variable("longMarketValue"));
  detail.short_market_value = MakeMoney(account.get_balance_variable("shortMarketValue"));
  detail.buying_power = MakeMoney(account.get_balance_variable("buyingPower"));
  detail.equity = MakeMoney(account.get_balance_variable("equity"));
  detail.equity_percentage = account.get_balance_variable("equityPercentage");
  detail.margin_balance = MakeMoney(account.get_balance_variable("marginBalance"));

  for (size_t i = 0; i < account.get_position_vector_size(); ++i) {
    auto position = account.get_position(static_cast<int>(i));
    const auto symbol = position.count("symbol") ? position["symbol"] : "UNKNOWN";
    auto name = position.count("description") ? position["description"] : symbol;
    if (name.empty()) {
      name = symbol;
    }
    detail.positions.push_back(
        {symbol,
         name,
         MakeMoney(account.get_position_balances(symbol, "currentDayProfitLoss")),
         account.get_position_balances(symbol, "currentDayProfitLossPercentage"),
         MakeMoney(account.get_position_balances(symbol, "averagePrice")),
         MakeMoney(account.get_position_balances(symbol, "marketValue")),
         account.get_position_balances(symbol, "longQuantity")});
  }

  return detail;
}

}  // namespace premia::providers::tda

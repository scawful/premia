#include "premia/providers/tda/portfolio_provider.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TDAmeritrade/client.h"
#include "TDAmeritrade/parser.h"
#include "TDAmeritrade/data/Account.hpp"

namespace premia::providers::tda {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount.empty() ? "0.00" : amount, "USD"};
}

auto MakeChange(const std::string& absolute, const std::string& percent)
    -> domain::AbsolutePercentChange {
  return domain::AbsolutePercentChange{
      MakeMoney(absolute.empty() ? "0.00" : absolute),
      percent.empty() ? "0.00" : percent};
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

auto ParseDouble(const std::string& value) -> double {
  try {
    return boost::lexical_cast<double>(value);
  } catch (const boost::bad_lexical_cast&) {
    return 0.0;
  }
}

}  // namespace

PortfolioProvider::PortfolioProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto PortfolioProvider::HasUsableConfig() const -> bool {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    return false;
  }

  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  return !IsPlaceholderValue(consumer_key) && !IsPlaceholderValue(refresh_token);
}

auto PortfolioProvider::LoadAccount() const -> ::premia::tda::Account {
  if (!HasUsableConfig()) {
    throw std::runtime_error("tda portfolio config unavailable");
  }

  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    throw std::runtime_error("unable to read tda portfolio config");
  }

  ::premia::tda::Client client;
  ::premia::tda::Parser parser;
  client.addAuth(tree.get<std::string>("consumer_key"),
                 tree.get<std::string>("refresh_token"));
  client.fetch_access_token();
  (void)client.get_all_accounts();

  const auto account_ids = client.get_all_account_ids();
  if (account_ids.empty()) {
    throw std::runtime_error("tda returned no accounts");
  }

  const auto response = client.get_account(account_ids.front());
  return parser.parse_account(parser.read_response(response));
}

auto PortfolioProvider::GetPortfolioSummary() const
    -> application::PortfolioSummary {
  auto account = LoadAccount();

  const auto total_value = account.get_balance_variable("liquidationValue");
  const auto cash = account.get_balance_variable("cashBalance");
  auto buying_power = account.get_balance_variable("buyingPower");
  if (buying_power.empty()) {
    buying_power = account.get_balance_variable("availableFunds");
  }
  auto day_change = account.get_balance_variable("currentDayProfitLoss");
  auto day_change_percent = account.get_balance_variable("currentDayProfitLossPercentage");
  if (day_change.empty()) {
    day_change = "0.00";
  }
  if (day_change_percent.empty()) {
    day_change_percent = "0.00";
  }

  return application::PortfolioSummary{
      MakeMoney(total_value.empty() ? "0.00" : total_value),
      MakeChange(day_change, day_change_percent),
      MakeMoney(cash.empty() ? "0.00" : cash),
      MakeMoney(buying_power.empty() ? "0.00" : buying_power),
      static_cast<int>(account.get_position_vector_size())};
}

auto PortfolioProvider::GetTopHoldings() const
    -> std::vector<application::HoldingRow> {
  auto account = LoadAccount();
  std::vector<application::HoldingRow> holdings;
  holdings.reserve(account.get_position_vector_size());

  for (size_t i = 0; i < account.get_position_vector_size(); ++i) {
    auto position = account.get_position(static_cast<int>(i));
    const auto symbol = position.count("symbol") ? position["symbol"] : "UNKNOWN";
    auto name = position.count("description") ? position["description"] : symbol;
    if (name.empty()) {
      name = symbol;
    }

    auto quantity = account.get_position_balances(symbol, "longQuantity");
    if (quantity.empty() && position.count("longQuantity")) {
      quantity = position["longQuantity"];
    }
    auto market_value = account.get_position_balances(symbol, "marketValue");
    if (market_value.empty() && position.count("marketValue")) {
      market_value = position["marketValue"];
    }
    auto day_change = account.get_position_balances(symbol, "currentDayProfitLoss");
    auto day_change_percent =
        account.get_position_balances(symbol, "currentDayProfitLossPercentage");

    holdings.push_back(
        {"tda:" + symbol,
         symbol,
         name,
         quantity.empty() ? "0" : quantity,
         MakeMoney(market_value.empty() ? "0.00" : market_value),
         MakeChange(day_change, day_change_percent)});
  }

  std::sort(holdings.begin(), holdings.end(),
            [](const application::HoldingRow& left,
               const application::HoldingRow& right) {
              return ParseDouble(left.market_value.amount) >
                     ParseDouble(right.market_value.amount);
            });

  return holdings;
}

}  // namespace premia::providers::tda

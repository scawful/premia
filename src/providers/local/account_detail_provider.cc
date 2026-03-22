#include "premia/providers/local/account_detail_provider.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace premia::providers::local {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace pt = boost::property_tree;

namespace {

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount, "USD"};
}

auto BuildFallback() -> application::AccountDetail {
  application::AccountDetail detail;
  detail.account_id = "local_acc";
  detail.cash = MakeMoney("14320.00");
  detail.net_liquidation = MakeMoney("128345.22");
  detail.available_funds = MakeMoney("28640.00");
  detail.long_market_value = MakeMoney("113024.22");
  detail.short_market_value = MakeMoney("0.00");
  detail.buying_power = MakeMoney("28640.00");
  detail.equity = MakeMoney("128345.22");
  detail.equity_percentage = "100.00";
  detail.margin_balance = MakeMoney("0.00");
  detail.positions = {
      {"AAPL", "Apple Inc.", MakeMoney("54.60"), "0.60", MakeMoney("200.50"),
       MakeMoney("9114.00"), "42"},
      {"MSFT", "Microsoft Corp.", MakeMoney("41.22"), "0.55", MakeMoney("398.20"),
       MakeMoney("7561.80"), "18"},
      {"SPY", "SPDR S&P 500 ETF", MakeMoney("92.50"), "0.72", MakeMoney("501.30"),
       MakeMoney("12987.50"), "25"},
  };
  return detail;
}

}  // namespace

AccountDetailProvider::AccountDetailProvider(std::string path)
    : path_(std::move(path)) {}

auto AccountDetailProvider::GetAccountDetail() const -> application::AccountDetail {
  std::ifstream file(path_);
  if (!file.good()) {
    return BuildFallback();
  }

  try {
    pt::ptree root;
    pt::read_json(file, root);
    const auto& tree = root.get_child("account");

    application::AccountDetail detail;
    detail.account_id = tree.get<std::string>("accountId", "local_acc");
    detail.cash = MakeMoney(tree.get<std::string>("cash", "14320.00"));
    detail.net_liquidation =
        MakeMoney(tree.get<std::string>("netLiquidation", "128345.22"));
    detail.available_funds =
        MakeMoney(tree.get<std::string>("availableFunds", "28640.00"));
    detail.long_market_value =
        MakeMoney(tree.get<std::string>("longMarketValue", "113024.22"));
    detail.short_market_value =
        MakeMoney(tree.get<std::string>("shortMarketValue", "0.00"));
    detail.buying_power = MakeMoney(tree.get<std::string>("buyingPower", "28640.00"));
    detail.equity = MakeMoney(tree.get<std::string>("equity", "128345.22"));
    detail.equity_percentage = tree.get<std::string>("equityPercentage", "100.00");
    detail.margin_balance = MakeMoney(tree.get<std::string>("marginBalance", "0.00"));

    for (const auto& item : tree.get_child("positions")) {
      const auto& position = item.second;
      detail.positions.push_back(
          {position.get<std::string>("symbol", ""),
           position.get<std::string>("name", ""),
           MakeMoney(position.get<std::string>("dayProfitLoss", "0.00")),
           position.get<std::string>("dayProfitLossPercent", "0.00"),
           MakeMoney(position.get<std::string>("averagePrice", "0.00")),
           MakeMoney(position.get<std::string>("marketValue", "0.00")),
           position.get<std::string>("quantity", "0")});
    }
    return detail;
  } catch (const std::exception&) {
    return BuildFallback();
  }
}

}  // namespace premia::providers::local

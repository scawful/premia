#include "premia/providers/local/portfolio_provider.hpp"

#include <fstream>
#include <string>
#include <vector>

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

auto MakeChange(const std::string& absolute, const std::string& percent)
    -> domain::AbsolutePercentChange {
  return domain::AbsolutePercentChange{MakeMoney(absolute), percent};
}

auto BuildFallbackSummary() -> application::PortfolioSummary {
  return application::PortfolioSummary{
      MakeMoney("128345.22"), MakeChange("842.13", "0.66"),
      MakeMoney("14320.00"), MakeMoney("28640.00"), 3};
}

auto BuildFallbackHoldings() -> std::vector<application::HoldingRow> {
  return {
      {"holding_aapl", "AAPL", "Apple Inc.", "42", MakeMoney("9114.00"),
       MakeChange("54.60", "0.60")},
      {"holding_msft", "MSFT", "Microsoft Corp.", "18",
       MakeMoney("7561.80"), MakeChange("41.22", "0.55")},
      {"holding_spy", "SPY", "SPDR S&P 500 ETF", "25", MakeMoney("12987.50"),
       MakeChange("92.50", "0.72")},
  };
}

auto ReadTree(const std::string& path, pt::ptree& tree) -> bool {
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

}  // namespace

PortfolioProvider::PortfolioProvider(std::string path) : path_(std::move(path)) {}

auto PortfolioProvider::GetPortfolioSummary() const -> application::PortfolioSummary {
  return LoadPortfolioSummary();
}

auto PortfolioProvider::GetTopHoldings() const
    -> std::vector<application::HoldingRow> {
  return LoadHoldings();
}

auto PortfolioProvider::LoadPortfolioSummary() const
    -> application::PortfolioSummary {
  pt::ptree root;
  if (!ReadTree(path_, root)) {
    return BuildFallbackSummary();
  }

  try {
    const auto& tree = root.get_child("portfolio");
    return application::PortfolioSummary{
        MakeMoney(tree.get<std::string>("totalValue", "128345.22")),
        MakeChange(tree.get<std::string>("dayChangeAbsolute", "842.13"),
                   tree.get<std::string>("dayChangePercent", "0.66")),
        MakeMoney(tree.get<std::string>("cash", "14320.00")),
        MakeMoney(tree.get<std::string>("buyingPower", "28640.00")),
        tree.get<int>("holdingsCount", 3)};
  } catch (const std::exception&) {
    return BuildFallbackSummary();
  }
}

auto PortfolioProvider::LoadHoldings() const
    -> std::vector<application::HoldingRow> {
  pt::ptree root;
  if (!ReadTree(path_, root)) {
    return BuildFallbackHoldings();
  }

  try {
    std::vector<application::HoldingRow> holdings;
    for (const auto& item : root.get_child("holdings")) {
      const auto& tree = item.second;
      holdings.push_back(
          {tree.get<std::string>("id", "holding"),
           tree.get<std::string>("symbol", ""),
           tree.get<std::string>("name", ""),
           tree.get<std::string>("quantity", "0"),
           MakeMoney(tree.get<std::string>("marketValue", "0.00")),
           MakeChange(tree.get<std::string>("dayChangeAbsolute", "0.00"),
                      tree.get<std::string>("dayChangePercent", "0.00"))});
    }
    return holdings.empty() ? BuildFallbackHoldings() : holdings;
  } catch (const std::exception&) {
    return BuildFallbackHoldings();
  }
}

}  // namespace premia::providers::local

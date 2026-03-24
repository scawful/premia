#include "premia/providers/local/options_provider.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace premia::providers::local {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

auto BuildFallback() -> application::OptionChainSnapshot {
  application::OptionChainSnapshot snapshot;
  snapshot.symbol = "SPY";
  snapshot.description = "SPDR S&P 500 ETF";
  snapshot.bid = "519.45";
  snapshot.ask = "519.54";
  snapshot.open_price = "516.00";
  snapshot.close_price = "515.80";
  snapshot.high_price = "520.10";
  snapshot.low_price = "514.90";
  snapshot.total_volume = "58213422";
  snapshot.volatility = "16.8";
  snapshot.gamma_exposure = "1245500.00";
  snapshot.expirations = {{"2026-04-17:26",
                           "2026-04-17:26",
                           "34200.00",
                           {{"520c", "520", "SPY_041726C520", "7.80", "7.90", "7.85", "0.42", "0.52", "0.03", "-0.08", "0.12", "845", "SPY_041726P520", "8.10", "8.20", "8.15", "0.36", "-0.48", "0.03", "-0.08", "0.11", "902"},
                            {"525c", "525", "SPY_041726C525", "5.20", "5.30", "5.25", "0.28", "0.41", "0.02", "-0.07", "0.10", "774", "SPY_041726P525", "10.60", "10.75", "10.68", "0.31", "-0.59", "0.02", "-0.07", "0.10", "811"}}}};
  return snapshot;
}

}  // namespace

OptionsProvider::OptionsProvider(std::string path) : path_(std::move(path)) {}

auto OptionsProvider::GetOptionChainSnapshot(const std::string& symbol,
                                             const std::string& strike_count,
                                             const std::string& strategy,
                                             const std::string& range,
                                             const std::string& exp_month,
                                             const std::string& option_type) const
    -> application::OptionChainSnapshot {
  (void)strike_count;
  (void)strategy;
  (void)range;
  (void)exp_month;
  (void)option_type;

  std::ifstream file(path_);
  if (!file.good()) {
    auto fallback = BuildFallback();
    if (!symbol.empty()) {
      fallback.symbol = symbol;
    }
    return fallback;
  }

  try {
    pt::ptree root;
    pt::read_json(file, root);
    application::OptionChainSnapshot snapshot;
    snapshot.symbol = root.get<std::string>("symbol", symbol.empty() ? "SPY" : symbol);
    snapshot.description = root.get<std::string>("description", snapshot.symbol);
    snapshot.bid = root.get<std::string>("bid", "0.00");
    snapshot.ask = root.get<std::string>("ask", "0.00");
    snapshot.open_price = root.get<std::string>("openPrice", "0.00");
    snapshot.close_price = root.get<std::string>("closePrice", "0.00");
    snapshot.high_price = root.get<std::string>("highPrice", "0.00");
    snapshot.low_price = root.get<std::string>("lowPrice", "0.00");
    snapshot.total_volume = root.get<std::string>("totalVolume", "0");
    snapshot.volatility = root.get<std::string>("volatility", "0.00");
    snapshot.gamma_exposure = root.get<std::string>("gammaExposure", "0.00");
    for (const auto& expiration_node : root.get_child("expirations")) {
      application::OptionExpirationSnapshot expiration;
      const auto& tree = expiration_node.second;
      expiration.id = tree.get<std::string>("id", "exp");
      expiration.label = tree.get<std::string>("label", expiration.id);
      expiration.gamma_at_expiry = tree.get<std::string>("gammaAtExpiry", "0.00");
      for (const auto& row_node : tree.get_child("rows")) {
        const auto& row = row_node.second;
        expiration.rows.push_back(
            {row.get<std::string>("id", "row"),
             row.get<std::string>("strike", "0"),
             row.get<std::string>("callSymbol", ""),
             row.get<std::string>("callBid", "0.00"),
             row.get<std::string>("callAsk", "0.00"),
             row.get<std::string>("callLast", "0.00"),
             row.get<std::string>("callChange", "0.00"),
             row.get<std::string>("callDelta", "0.00"),
             row.get<std::string>("callGamma", "0.00"),
             row.get<std::string>("callTheta", "0.00"),
             row.get<std::string>("callVega", "0.00"),
             row.get<std::string>("callOpenInterest", "0"),
             row.get<std::string>("putSymbol", ""),
             row.get<std::string>("putBid", "0.00"),
             row.get<std::string>("putAsk", "0.00"),
             row.get<std::string>("putLast", "0.00"),
             row.get<std::string>("putChange", "0.00"),
             row.get<std::string>("putDelta", "0.00"),
             row.get<std::string>("putGamma", "0.00"),
             row.get<std::string>("putTheta", "0.00"),
             row.get<std::string>("putVega", "0.00"),
             row.get<std::string>("putOpenInterest", "0")});
      }
      snapshot.expirations.push_back(expiration);
    }
    return snapshot;
  } catch (const std::exception&) {
    auto fallback = BuildFallback();
    if (!symbol.empty()) {
      fallback.symbol = symbol;
    }
    return fallback;
  }
}

}  // namespace premia::providers::local

#include "premia/providers/tda/options_provider.hpp"

#include <fstream>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "TDAmeritrade/client.h"
#include "TDAmeritrade/data/OptionChain.hpp"
#include "TDAmeritrade/parser.h"

namespace premia::providers::tda {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

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

auto GetRaw(const premia::tda::StrikePriceMap& strike, const std::string& key)
    -> std::string {
  auto it = strike.raw_option.find(key);
  return it == strike.raw_option.end() ? "0.00" : it->second;
}

}  // namespace

OptionsProvider::OptionsProvider(std::string config_path)
    : config_path_(std::move(config_path)) {}

auto OptionsProvider::GetOptionChainSnapshot(const std::string& symbol,
                                             const std::string& strike_count,
                                             const std::string& strategy,
                                             const std::string& range,
                                             const std::string& exp_month,
                                             const std::string& option_type) const
    -> application::OptionChainSnapshot {
  pt::ptree tree;
  if (!ReadConfigTree(config_path_, tree)) {
    throw std::runtime_error("tda options config unavailable");
  }
  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  if (IsPlaceholderValue(consumer_key) || IsPlaceholderValue(refresh_token)) {
    throw std::runtime_error("tda options config is placeholder");
  }

  premia::tda::Client client;
  premia::tda::Parser parser;
  client.addAuth(consumer_key, refresh_token);
  client.fetch_access_token();
  const auto response = client.get_option_chain(symbol, "ALL", strike_count, true,
                                                strategy, range, exp_month, option_type);
  auto chain = parser.parse_option_chain(parser.read_response(response));

  application::OptionChainSnapshot snapshot;
  snapshot.symbol = chain.getOptionChainDataVariable("symbol");
  snapshot.description = chain.getUnderlyingDataVariable("description");
  snapshot.bid = chain.getUnderlyingDataVariable("bid");
  snapshot.ask = chain.getUnderlyingDataVariable("ask");
  snapshot.open_price = chain.getUnderlyingDataVariable("openPrice");
  snapshot.close_price = chain.getUnderlyingDataVariable("close");
  snapshot.high_price = chain.getUnderlyingDataVariable("highPrice");
  snapshot.low_price = chain.getUnderlyingDataVariable("lowPrice");
  snapshot.total_volume = chain.getUnderlyingDataVariable("totalVolume");
  snapshot.volatility = chain.getOptionChainDataVariable("volatility");
  snapshot.gamma_exposure = "0.00";

  const auto calls = chain.getCallOptionArray();
  const auto puts = chain.getPutOptionArray();
  for (size_t i = 0; i < calls.size(); ++i) {
    application::OptionExpirationSnapshot expiration;
    expiration.id = calls[i].datetime;
    expiration.label = calls[i].datetime;
    expiration.gamma_at_expiry = "0.00";
    const auto& puts_for_date = i < puts.size() ? puts[i] : premia::tda::OptionsDateTimeObj{};
    for (size_t row = 0; row < calls[i].strikePriceObj.size(); ++row) {
      const auto& call_strike = calls[i].strikePriceObj[row];
      const auto has_put = row < puts_for_date.strikePriceObj.size();
      const auto& put_strike = has_put ? puts_for_date.strikePriceObj[row] : call_strike;
      expiration.rows.push_back(
          {calls[i].datetime + ":" + call_strike.strikePrice,
           call_strike.strikePrice,
           GetRaw(call_strike, "symbol"),
           GetRaw(call_strike, "bid"),
           GetRaw(call_strike, "ask"),
           GetRaw(call_strike, "last"),
           GetRaw(call_strike, "netChange"),
           GetRaw(call_strike, "delta"),
           GetRaw(call_strike, "gamma"),
           GetRaw(call_strike, "theta"),
           GetRaw(call_strike, "vega"),
           GetRaw(call_strike, "openInterest"),
           has_put ? GetRaw(put_strike, "symbol") : "",
           has_put ? GetRaw(put_strike, "bid") : "0.00",
           has_put ? GetRaw(put_strike, "ask") : "0.00",
           has_put ? GetRaw(put_strike, "last") : "0.00",
           has_put ? GetRaw(put_strike, "netChange") : "0.00",
           has_put ? GetRaw(put_strike, "delta") : "0.00",
           has_put ? GetRaw(put_strike, "gamma") : "0.00",
           has_put ? GetRaw(put_strike, "theta") : "0.00",
           has_put ? GetRaw(put_strike, "vega") : "0.00",
           has_put ? GetRaw(put_strike, "openInterest") : "0"});
    }
    snapshot.expirations.push_back(expiration);
  }

  return snapshot;
}

}  // namespace premia::providers::tda

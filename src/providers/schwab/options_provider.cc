#include "premia/providers/schwab/options_provider.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Schwab/client.h"

namespace premia::providers::schwab {

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

auto ToStringInt(int value) -> std::string {
  return std::to_string(value);
}

auto ToStringDouble(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

auto GetString(const pt::ptree& tree, const std::string& path,
               const std::string& fallback = "") -> std::string {
  return tree.get<std::string>(path, fallback);
}

auto GetDouble(const pt::ptree& tree, const std::string& path,
               double fallback = 0.0) -> double {
  return tree.get<double>(path, fallback);
}

auto GetFirstContract(const pt::ptree* strike_node) -> const pt::ptree* {
  if (strike_node == nullptr) {
    return nullptr;
  }
  for (const auto& contract : *strike_node) {
    return &contract.second;
  }
  return nullptr;
}

auto ContractString(const pt::ptree* contract, const std::string& key,
                    const std::string& fallback = "0.00") -> std::string {
  if (contract == nullptr) {
    return fallback;
  }
  return contract->get<std::string>(key, fallback);
}

}  // namespace

OptionsProvider::OptionsProvider(std::string config_path, std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto OptionsProvider::GetOptionChainSnapshot(const std::string& symbol,
                                             const std::string& strike_count,
                                             const std::string& strategy,
                                             const std::string& range,
                                             const std::string& exp_month,
                                             const std::string& option_type) const
    -> application::OptionChainSnapshot {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client, config_path_, token_path_)) {
    throw std::runtime_error("schwab client unavailable");
  }

  int strike_count_value = 8;
  try { strike_count_value = boost::lexical_cast<int>(strike_count); } catch (...) {}

  const auto response = client.GetOptionChain(symbol, "ALL", strike_count_value,
                                              strategy.empty() ? "SINGLE" : strategy,
                                              range.empty() ? "ALL" : range,
                                              exp_month.empty() ? "ALL" : exp_month,
                                              option_type.empty() ? "ALL" : option_type,
                                              true);
  if (response.empty()) {
    throw std::runtime_error("empty schwab options response");
  }

  std::istringstream ss(response);
  pt::ptree root;
  pt::read_json(ss, root);

  application::OptionChainSnapshot snapshot;
  snapshot.symbol = root.get<std::string>("symbol", symbol);
  snapshot.description = root.get<std::string>("underlying.description", symbol);
  snapshot.bid = root.get<std::string>("underlying.bid", "0.00");
  snapshot.ask = root.get<std::string>("underlying.ask", "0.00");
  snapshot.open_price = root.get<std::string>("underlying.openPrice", "0.00");
  snapshot.close_price = root.get<std::string>("underlying.close", "0.00");
  snapshot.high_price = root.get<std::string>("underlying.highPrice", "0.00");
  snapshot.low_price = root.get<std::string>("underlying.lowPrice", "0.00");
  snapshot.total_volume = root.get<std::string>("underlying.totalVolume", "0");
  snapshot.volatility = ToStringDouble(root.get<double>("volatility", 0.0));
  snapshot.gamma_exposure = "0.00";

  auto call_map = root.get_child_optional("callExpDateMap");
  auto put_map = root.get_child_optional("putExpDateMap");
  if (!call_map) {
    return snapshot;
  }

  for (const auto& exp_entry : call_map.get()) {
    application::OptionExpirationSnapshot expiration;
    expiration.id = exp_entry.first;
    expiration.label = exp_entry.first;
    expiration.gamma_at_expiry = "0.00";
    const pt::ptree* puts_for_exp = nullptr;
    if (put_map) {
      auto maybe_puts = put_map->find(exp_entry.first);
      if (maybe_puts != put_map->not_found()) {
        puts_for_exp = &maybe_puts->second;
      }
    }

    for (const auto& strike_entry : exp_entry.second) {
      const auto strike = strike_entry.first;
      const auto* call_contract = GetFirstContract(&strike_entry.second);
      const pt::ptree* put_strike = nullptr;
      if (puts_for_exp != nullptr) {
        auto maybe_put = puts_for_exp->find(strike);
        if (maybe_put != puts_for_exp->not_found()) {
          put_strike = &maybe_put->second;
        }
      }
      const auto* put_contract = GetFirstContract(put_strike);

      expiration.rows.push_back(
          {exp_entry.first + ":" + strike,
           strike,
           ContractString(call_contract, "symbol", ""),
           ContractString(call_contract, "bid"),
           ContractString(call_contract, "ask"),
           ContractString(call_contract, "last"),
           ContractString(call_contract, "netChange"),
           ContractString(call_contract, "delta"),
           ContractString(call_contract, "gamma"),
           ContractString(call_contract, "theta"),
           ContractString(call_contract, "vega"),
           ContractString(call_contract, "openInterest", "0"),
           ContractString(put_contract, "symbol", ""),
           ContractString(put_contract, "bid"),
           ContractString(put_contract, "ask"),
           ContractString(put_contract, "last"),
           ContractString(put_contract, "netChange"),
           ContractString(put_contract, "delta"),
           ContractString(put_contract, "gamma"),
           ContractString(put_contract, "theta"),
           ContractString(put_contract, "vega"),
           ContractString(put_contract, "openInterest", "0")});
    }

    snapshot.expirations.push_back(expiration);
  }

  return snapshot;
}

}  // namespace premia::providers::schwab

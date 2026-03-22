#include "premia/providers/schwab/market_data_provider.hpp"

#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
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

struct HistoryRequest {
  std::string period_type;
  int period_amount = 1;
  std::string frequency_type;
  int frequency_amount = 1;
};

auto MakeMoney(const std::string& amount) -> domain::Money {
  return domain::Money{amount, "USD"};
}

auto MakeChange(const std::string& absolute, const std::string& percent)
    -> domain::AbsolutePercentChange {
  return domain::AbsolutePercentChange{MakeMoney(absolute), percent};
}

auto IsPlaceholderValue(const std::string& value) -> bool {
  return value.empty() || value.rfind("YOUR_", 0) == 0;
}

auto ReadJsonTree(const std::string& path, pt::ptree& tree) -> bool {
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
  if (!ReadJsonTree(path, tree)) {
    return false;
  }

  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto GetTreeString(const pt::ptree& tree,
                   std::initializer_list<std::string> paths,
                   const std::string& fallback = "") -> std::string {
  for (const auto& path : paths) {
    auto value = tree.get_optional<std::string>(pt::ptree::path_type(path, '.'));
    if (value && !value->empty()) {
      return *value;
    }
  }
  return fallback;
}

auto GetTreeDouble(const pt::ptree& tree,
                   std::initializer_list<std::string> paths,
                   double fallback = 0.0) -> double {
  for (const auto& path : paths) {
    auto value = tree.get_optional<double>(pt::ptree::path_type(path, '.'));
    if (value) {
      return *value;
    }
  }
  return fallback;
}

auto FormatDouble(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

auto CurrentUtcTimestampString() -> std::string {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_time{};
#if defined(_WIN32)
  gmtime_s(&utc_time, &now_time);
#else
  gmtime_r(&now_time, &utc_time);
#endif
  std::ostringstream oss;
  oss << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

auto NormalizeRangeToken(std::string token) -> std::string {
  std::string normalized;
  normalized.reserve(token.size());
  for (const char ch : token) {
    if (ch != ' ' && ch != '_') {
      normalized.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
    }
  }
  return normalized;
}

auto BuildHistoryRequest(const std::string& range, const std::string& interval,
                         bool extended_hours) -> HistoryRequest {
  const auto normalized_range = NormalizeRangeToken(range);
  const auto normalized_interval = NormalizeRangeToken(interval);

  HistoryRequest request;
  request.period_type = "month";
  request.period_amount = 1;
  request.frequency_type = extended_hours ? "minute" : "daily";
  request.frequency_amount = extended_hours ? 5 : 1;

  if (normalized_range == "1D" || normalized_range == "DAY:1") {
    request.period_type = "day";
    request.period_amount = 1;
    request.frequency_type = "minute";
    request.frequency_amount = 5;
  } else if (normalized_range == "5D" || normalized_range == "DAY:5") {
    request.period_type = "day";
    request.period_amount = 5;
    request.frequency_type = "minute";
    request.frequency_amount = 15;
  } else if (normalized_range == "3M" || normalized_range == "MONTH:3") {
    request.period_type = "month";
    request.period_amount = 3;
  } else if (normalized_range == "6M" || normalized_range == "MONTH:6") {
    request.period_type = "month";
    request.period_amount = 6;
  } else if (normalized_range == "1Y" || normalized_range == "YEAR:1") {
    request.period_type = "year";
    request.period_amount = 1;
  } else if (normalized_range == "5Y" || normalized_range == "YEAR:5") {
    request.period_type = "year";
    request.period_amount = 5;
    request.frequency_type = "weekly";
    request.frequency_amount = 1;
  } else if (normalized_range == "MAX") {
    request.period_type = "year";
    request.period_amount = 20;
    request.frequency_type = "monthly";
    request.frequency_amount = 1;
  }

  if (normalized_interval == "1M" || normalized_interval == "MINUTE:1") {
    request.frequency_type = "minute";
    request.frequency_amount = 1;
  } else if (normalized_interval == "5M" || normalized_interval == "MINUTE:5") {
    request.frequency_type = "minute";
    request.frequency_amount = 5;
  } else if (normalized_interval == "15M" || normalized_interval == "MINUTE:15") {
    request.frequency_type = "minute";
    request.frequency_amount = 15;
  } else if (normalized_interval == "1H") {
    request.frequency_type = "minute";
    request.frequency_amount = 30;
  } else if (normalized_interval == "1W" || normalized_interval == "WEEKLY:1") {
    request.frequency_type = "weekly";
    request.frequency_amount = 1;
  }

  return request;
}

}  // namespace

MarketDataProvider::MarketDataProvider(std::string config_path,
                                       std::string token_path)
    : config_path_(std::move(config_path)), token_path_(std::move(token_path)) {}

auto MarketDataProvider::LoadAuthorizedClient(::premia::schwab::Client& client) const
    -> bool {
  if (!HasUsableConfig(config_path_)) {
    return false;
  }
  if (!client.LoadConfig(config_path_)) {
    return false;
  }
  client.LoadTokens(token_path_);
  if (client.HasValidAccessToken()) {
    return true;
  }
  if (client.HasValidRefreshToken() && client.RefreshAccessToken()) {
    client.SaveTokens(token_path_);
    return client.HasValidAccessToken();
  }
  return false;
}

auto MarketDataProvider::GetQuoteDetail(const std::string& symbol) const
    -> application::QuoteDetail {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client)) {
    throw std::runtime_error("schwab client unavailable");
  }

  const auto response = client.GetQuote(symbol);
  if (response.empty()) {
    throw std::runtime_error("empty schwab quote response");
  }

  std::istringstream ss(response);
  pt::ptree root;
  pt::read_json(ss, root);
  const auto* symbol_node = &root;
  if (auto child = root.get_child_optional(symbol)) {
    symbol_node = &child.get();
  }

  application::QuoteDetail detail;
  detail.instrument.symbol = GetTreeString(*symbol_node, {"symbol"}, symbol);
  detail.instrument.name = GetTreeString(
      *symbol_node, {"description", "reference.description", "quote.description"},
      symbol);
  detail.instrument.asset_type = "equity";
  detail.instrument.primary_exchange = GetTreeString(
      *symbol_node, {"quote.exchangeName", "quote.exchange", "reference.exchange"},
      "NASDAQ");

  detail.quote.last_price =
      MakeMoney(FormatDouble(GetTreeDouble(*symbol_node, {"quote.lastPrice", "lastPrice", "quote.mark"}, 0.0)));
  detail.quote.bid = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.bidPrice", "bidPrice"}, 0.0)));
  detail.quote.ask = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.askPrice", "askPrice"}, 0.0)));
  detail.quote.open = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.openPrice", "openPrice"}, 0.0)));
  detail.quote.high = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.highPrice", "highPrice"}, 0.0)));
  detail.quote.low = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.lowPrice", "lowPrice"}, 0.0)));
  detail.quote.previous_close = MakeMoney(FormatDouble(
      GetTreeDouble(*symbol_node, {"quote.closePrice", "closePrice"}, 0.0)));
  detail.quote.volume = GetTreeString(*symbol_node, {"quote.totalVolume", "totalVolume"},
                                      "0");
  detail.quote.updated_at = CurrentUtcTimestampString();
  return detail;
}

auto MarketDataProvider::GetChartScreen(const std::string& symbol,
                                        const std::string& range,
                                        const std::string& interval,
                                        bool extended_hours) const
    -> application::ChartScreenData {
  ::premia::schwab::Client client;
  if (!LoadAuthorizedClient(client)) {
    throw std::runtime_error("schwab client unavailable");
  }

  const auto request = BuildHistoryRequest(range, interval, extended_hours);
  const auto response = client.GetPriceHistory(symbol, request.period_type,
                                               request.period_amount,
                                               request.frequency_type,
                                               request.frequency_amount,
                                               extended_hours);
  if (response.empty()) {
    throw std::runtime_error("empty schwab chart response");
  }

  std::istringstream ss(response);
  pt::ptree root;
  pt::read_json(ss, root);
  auto candles_opt = root.get_child_optional("candles");
  if (!candles_opt) {
    throw std::runtime_error("missing schwab candles");
  }

  application::ChartScreenData data;
  data.instrument = {symbol, GetTreeString(root, {"symbol"}, symbol), "equity",
                     "NASDAQ"};
  data.range = range.empty() ? "1M" : range;
  data.interval = interval.empty() ? "1D" : interval;
  data.timezone = "America/New_York";
  data.series.type = extended_hours ? "line" : "candles";

  double first_close = 0.0;
  double last_close = 0.0;
  bool have_close = false;

  for (const auto& item : candles_opt.get()) {
    const auto& candle = item.second;
    const auto open = candle.get<double>("open", 0.0);
    const auto high = candle.get<double>("high", 0.0);
    const auto low = candle.get<double>("low", 0.0);
    const auto close = candle.get<double>("close", 0.0);
    const auto volume = candle.get<long long>("volume", 0);
    const auto time = candle.get<long long>("datetime", 0);

    if (!have_close) {
      first_close = close;
      have_close = true;
    }
    last_close = close;

    data.series.bars.push_back(
        {std::to_string(time), FormatDouble(open), FormatDouble(high),
         FormatDouble(low), FormatDouble(close), std::to_string(volume)});
  }

  if (have_close && first_close != 0.0) {
    const auto absolute = last_close - first_close;
    const auto percent = (absolute / first_close) * 100.0;
    data.stats = application::ChartStats{MakeChange(FormatDouble(absolute),
                                                    FormatDouble(percent))};
  }

  return data;
}

}  // namespace premia::providers::schwab

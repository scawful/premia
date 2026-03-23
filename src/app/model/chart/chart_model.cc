#include "chart_model.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "premia/core/application/composition_root.hpp"

namespace premia {

namespace {

auto ParseDecimal(const std::string& value) -> double {
  try {
    return boost::lexical_cast<double>(value);
  } catch (const boost::bad_lexical_cast&) {
    return 0.0;
  }
}

auto ParseTimestamp(const std::string& value) -> double {
  if (value.empty()) {
    return 0.0;
  }

  const bool numeric = std::all_of(value.begin(), value.end(), [](unsigned char ch) {
    return std::isdigit(ch) != 0;
  });
  if (numeric) {
    auto timestamp = ParseDecimal(value);
    if (timestamp > 1000000000000.0) {
      timestamp *= 0.001;
    }
    return timestamp;
  }

  std::tm parsed{};
  std::istringstream stream(value);
  stream >> std::get_time(&parsed, "%Y-%m-%dT%H:%M:%SZ");
  if (stream.fail()) {
    return 0.0;
  }
#if defined(_WIN32)
  return static_cast<double>(_mkgmtime(&parsed));
#else
  return static_cast<double>(timegm(&parsed));
#endif
}

auto MakeRangeLabel(tda::PeriodType type, int amount) -> std::string {
  static const char* kTypes[] = {"Day", "Month", "Year", "YTD"};
  const auto index = static_cast<int>(type);
  const auto label = (index >= 0 && index < 4) ? kTypes[index] : "Year";
  return std::string(label) + ":" + std::to_string(amount);
}

auto MakeIntervalLabel(tda::FrequencyType type, int amount) -> std::string {
  static const char* kTypes[] = {"Minute", "Daily", "Weekly", "Monthly"};
  const auto index = static_cast<int>(type);
  const auto label = (index >= 0 && index < 4) ? kTypes[index] : "Daily";
  return std::string(label) + ":" + std::to_string(amount);
}

}  // namespace

void ChartModel::initCandles() {
  candles.clear();
  datesVec.clear();
  volumeVec.clear();
  lowBound = 0.0;
  highBound = 0.0;

  for (const auto& bar : chartData.series.bars) {
    tda::Candle candle{};
    candle.open = ParseDecimal(bar.open);
    candle.high = ParseDecimal(bar.high);
    candle.low = ParseDecimal(bar.low);
    candle.close = ParseDecimal(bar.close);
    candle.volume = ParseDecimal(bar.volume);
    candle.datetime = bar.time;
    candle.raw_datetime = static_cast<time_t>(ParseTimestamp(bar.time));
    candles.push_back(candle);

    const auto timestamp = ParseTimestamp(bar.time);
    datesVec.push_back(timestamp);
    volumeVec.push_back(candle.volume);

    if (lowBound == 0.0 || candle.low < lowBound) {
      lowBound = candle.low;
    }
    if (candle.high > highBound) {
      highBound = candle.high;
    }
  }

  const auto quoteLow = ParseDecimal(quoteDetail.quote.low.amount);
  const auto quoteHigh = ParseDecimal(quoteDetail.quote.high.amount);
  if (lowBound == 0.0 || (quoteLow > 0.0 && quoteLow < lowBound)) {
    lowBound = quoteLow;
  }
  if (quoteHigh > highBound) {
    highBound = quoteHigh;
  }
}

std::string ChartModel::getQuoteDetails() {
  return "Bid: $" + quoteDetail.quote.bid.amount + " | Ask: $" +
         quoteDetail.quote.ask.amount + " | Open: $" +
         quoteDetail.quote.open.amount + " | Close: $" +
         quoteDetail.quote.previous_close.amount;
}

void ChartModel::fetchPriceHistory(const std::string& ticker,
                                   tda::PeriodType ptype, int period_amt,
                                   tda::FrequencyType ftype, int freq_amt,
                                   bool ext) {
  tickerSymbol = ticker;
  auto& services = core::application::CompositionRoot::Instance();
  quoteDetail = services.MarketData().GetQuoteDetail(ticker);
  chartData = services.MarketData().GetChartScreen(
      ticker, MakeRangeLabel(ptype, period_amt), MakeIntervalLabel(ftype, freq_amt), ext);
  initCandles();
  active = true;
}

}  // namespace premia

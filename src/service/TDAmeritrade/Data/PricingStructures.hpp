#ifndef PricingStructures_hpp
#define PricingStructures_hpp

#include <string>
#include <unordered_map>
#include <vector>

namespace premia {
namespace tda {

struct Candle {
  double volume;
  double high;
  double low;
  double open;
  double close;
  std::string datetime;
  time_t raw_datetime;
};

struct OptionsContract {
  std::string putCall;
  double bid;
  double ask;
  double bidSize;
  double askSize;
  double open;
  double close;
  double high;
  double low;
  double delta;
  double gamma;
  double theta;
  double vega;
  double rho;
};

struct StrikePriceMap {
  std::string strikePrice;
  std::unordered_map<std::string, std::string> raw_option;
  std::unordered_map<std::string, OptionsContract> options;
};

struct OptionsDateTimeObj {
  std::string datetime;
  std::vector<StrikePriceMap> strikePriceObj;
};

enum PeriodType { DAY, MONTH, YEAR, YTD };

enum FrequencyType { MINUTE, DAILY, WEEKLY, MONTHLY };

enum OrderType {
  MARKET,
  LIMIT,
  STOP,
  STOP_LIMIT,
  TRAILING_STOP,
  MARKET_ON_CLOSE,
  EXERCISE,
  TRAILING_STOP_LIMIT,
  NET_DEBIT,
  NET_CREDIT,
  NET_ZERO
};

enum OrderStatus { ACCEPTED, WORKING, REJECTED, CANCELED };

enum ServiceType {
  NONE,
  ADMIN,
  ACTIVES_NASDAQ,
  ACTIVES_NYSE,
  ACTIVES_OTCBB,
  ACTIVES_OPTIONS,
  CHART_EQUITY,
  CHART_FOREX,
  CHART_FUTURES,
  CHART_OPTIONS,
  QUOTE,
  LEVELONE_FUTURES,
  LEVELONE_FOREX,
  LEVELONE_FUTURES_OPTIONS,
  OPTION,
  NEWS_HEADLINE,
  TIMESALE_EQUITY,
  TIMESALE_FUTURES,
  TIMESALE_FOREX,
  TIMESALE_OPTIONS,
  ACCT_ACTIVITY,
  CHART_HISTORY_FUTURES,
  FOREX_BOOK,
  FUTURES_BOOK,
  LISTED_BOOK,
  NASDAQ_BOOK,
  OPTIONS_BOOK,
  FUTURES_OPTIONS_BOOK,
  NEWS_STORY,
  NEWS_HEADLINE_LIST,
  UNKNOWN,
};

}  // namespace tda
}  // namespace premia
#endif
#ifndef PREMIA_CORE_APPLICATION_SCREEN_MODELS_HPP
#define PREMIA_CORE_APPLICATION_SCREEN_MODELS_HPP

#include <map>
#include <string>
#include <vector>

#include "premia/core/domain/value_types.hpp"

namespace premia::core::application {

using premia::core::domain::AbsolutePercentChange;
using premia::core::domain::ConnectionStatus;
using premia::core::domain::Money;
using premia::core::domain::Provider;

struct ConnectionSummary {
  Provider provider = Provider::kInternal;
  ConnectionStatus status = ConnectionStatus::kNotConnected;
  std::string display_name;
  std::string last_sync_at;
  bool reauth_required = false;
  std::map<std::string, bool> capabilities;
};

struct PortfolioSummary {
  Money total_value;
  AbsolutePercentChange day_change;
  Money cash;
  Money buying_power;
  int holdings_count = 0;
};

struct HoldingRow {
  std::string id;
  std::string symbol;
  std::string name;
  std::string quantity;
  Money market_value;
  AbsolutePercentChange day_change;
};

struct WatchlistSummary {
  std::string id;
  std::string name;
  int instrument_count = 0;
};

struct WatchlistRow {
  std::string id;
  std::string symbol;
  std::string name;
  Money last_price;
  AbsolutePercentChange day_change;
  Money bid;
  Money ask;
  std::string updated_at;
};

struct Instrument {
  std::string symbol;
  std::string name;
  std::string asset_type;
  std::string primary_exchange;
};

struct QuoteSnapshot {
  Money last_price;
  Money bid;
  Money ask;
  Money open;
  Money high;
  Money low;
  Money previous_close;
  std::string volume;
  std::string updated_at;
};

struct PositionSummary {
  std::string quantity;
  Money market_value;
  Money cost_basis;
  Money unrealized_gain;
};

struct WatchlistMembership {
  bool is_saved = false;
  std::vector<std::string> watchlist_ids;
};

struct QuoteDetail {
  Instrument instrument;
  QuoteSnapshot quote;
  PositionSummary position;
  WatchlistMembership watchlist_membership;
};

struct Candle {
  std::string time;
  std::string open;
  std::string high;
  std::string low;
  std::string close;
  std::string volume;
};

struct ChartSeries {
  std::string type;
  std::vector<Candle> bars;
};

struct ChartStats {
  AbsolutePercentChange change;
};

struct MarketSummary {
  std::string session;
  std::string next_transition_at;
};

struct BootstrapData {
  std::string environment;
  std::map<std::string, bool> feature_flags;
  std::vector<ConnectionSummary> connections;
};

struct HomeScreenData {
  std::vector<ConnectionSummary> connections;
  PortfolioSummary portfolio;
  std::vector<HoldingRow> top_holdings;
  std::vector<WatchlistSummary> watchlists;
  MarketSummary market;
};

struct WatchlistScreenData {
  WatchlistSummary watchlist;
  std::vector<WatchlistSummary> available_watchlists;
  std::vector<WatchlistRow> rows;
};

struct ChartScreenData {
  Instrument instrument;
  std::string range;
  std::string interval;
  std::string timezone;
  ChartSeries series;
  ChartStats stats;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SCREEN_MODELS_HPP

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

struct BrokerageAccountSummary {
  Provider provider = Provider::kInternal;
  std::string account_id;
  std::string display_name;
  Money total_value;
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
  bool is_archived = false;
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
  bool is_pinned = false;
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

struct ChartAnnotation {
  std::string id;
  std::string label;
  std::string price;
  std::string kind;
};

struct ChartAnnotationVersionSummary {
  std::string id;
  std::string saved_at;
  int annotation_count = 0;
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
  std::vector<BrokerageAccountSummary> brokerage_accounts;
  std::string active_account_id;
  PortfolioSummary portfolio;
  std::vector<HoldingRow> top_holdings;
  std::vector<WatchlistSummary> watchlists;
  MarketSummary market;
};

struct MultiAccountSummaryRow {
  Provider provider = Provider::kInternal;
  std::string account_id;
  std::string display_name;
  Money balance;
  AbsolutePercentChange day_change;
  int holdings_count = 0;
};

struct MultiAccountHomeScreen {
  std::vector<ConnectionSummary> connections;
  Money aggregate_net_worth;
  AbsolutePercentChange aggregate_day_change;
  std::vector<MultiAccountSummaryRow> accounts;
  std::vector<HoldingRow> top_holdings;
};

struct WatchlistScreenData {
  WatchlistSummary watchlist;
  std::vector<WatchlistSummary> available_watchlists;
  std::vector<WatchlistRow> rows;
};

struct AccountPositionRow {
  std::string symbol;
  std::string name;
  Money day_profit_loss;
  std::string day_profit_loss_percent;
  Money average_price;
  Money market_value;
  std::string quantity;
};

struct AccountDetail {
  std::string account_id;
  Money cash;
  Money net_liquidation;
  Money available_funds;
  Money long_market_value;
  Money short_market_value;
  Money buying_power;
  Money equity;
  std::string equity_percentage;
  Money margin_balance;
  std::vector<AccountPositionRow> positions;
};

struct OptionContractPairRow {
  std::string id;
  std::string strike;
  std::string call_symbol;
  std::string call_bid;
  std::string call_ask;
  std::string call_last;
  std::string call_change;
  std::string call_delta;
  std::string call_gamma;
  std::string call_theta;
  std::string call_vega;
  std::string call_open_interest;
  std::string put_symbol;
  std::string put_bid;
  std::string put_ask;
  std::string put_last;
  std::string put_change;
  std::string put_delta;
  std::string put_gamma;
  std::string put_theta;
  std::string put_vega;
  std::string put_open_interest;
};

struct OptionExpirationSnapshot {
  std::string id;
  std::string label;
  std::string gamma_at_expiry;
  std::vector<OptionContractPairRow> rows;
};

struct OptionChainSnapshot {
  std::string symbol;
  std::string description;
  std::string bid;
  std::string ask;
  std::string open_price;
  std::string close_price;
  std::string high_price;
  std::string low_price;
  std::string total_volume;
  std::string volatility;
  std::string gamma_exposure;
  std::vector<OptionExpirationSnapshot> expirations;
};

struct ChartScreenData {
  Instrument instrument;
  std::string range;
  std::string interval;
  std::string timezone;
  ChartSeries series;
  ChartStats stats;
  std::vector<ChartAnnotation> annotations;
  std::vector<ChartAnnotationVersionSummary> annotation_versions;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SCREEN_MODELS_HPP

#include "http/json.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <boost/json.hpp>
#include <boost/json/src.hpp>

#include "premia/core/domain/value_types.hpp"

namespace premia::api::http {

namespace application = premia::core::application;
namespace domain = premia::core::domain;
namespace json = boost::json;

namespace {

std::atomic<unsigned long long> g_request_id{0};

auto NextRequestId() -> std::string {
  return "req_" + std::to_string(++g_request_id);
}

auto CurrentUtcTimestamp() -> std::string {
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

auto MakeMeta() -> json::object {
  return {
      {"requestId", NextRequestId()},
      {"asOf", CurrentUtcTimestamp()},
  };
}

auto MakeErrorMeta() -> json::object {
  return {
      {"requestId", NextRequestId()},
  };
}

auto MakeMoney(const domain::Money& money) -> json::object {
  return {
      {"amount", money.amount},
      {"currency", money.currency},
  };
}

auto MakeAbsolutePercentChange(const domain::AbsolutePercentChange& change)
    -> json::object {
  return {
      {"absolute", MakeMoney(change.absolute)},
      {"percent", change.percent},
  };
}

auto MakeCapabilities(const std::map<std::string, bool>& capabilities)
    -> json::object {
  json::object object;
  for (const auto& capability : capabilities) {
    object[capability.first] = capability.second;
  }
  return object;
}

auto MakeConnectionSummary(const application::ConnectionSummary& summary)
    -> json::object {
  json::object object{{"provider", domain::ProviderToString(summary.provider)},
                      {"status", domain::ConnectionStatusToString(summary.status)},
                      {"displayName", summary.display_name},
                      {"reauthRequired", summary.reauth_required},
                      {"capabilities", MakeCapabilities(summary.capabilities)}};
  if (summary.last_sync_at.empty()) {
    object["lastSyncAt"] = nullptr;
  } else {
    object["lastSyncAt"] = summary.last_sync_at;
  }
  return object;
}

auto MakeConnectionSummaryArray(
    const std::vector<application::ConnectionSummary>& summaries) -> json::array {
  json::array array;
  for (const auto& summary : summaries) {
    array.emplace_back(MakeConnectionSummary(summary));
  }
  return array;
}

auto MakeWatchlistSummary(const application::WatchlistSummary& watchlist)
    -> json::object {
  return {
      {"id", watchlist.id},
      {"name", watchlist.name},
      {"instrumentCount", watchlist.instrument_count},
  };
}

auto MakeWatchlistSummaryArray(
    const std::vector<application::WatchlistSummary>& watchlists) -> json::array {
  json::array array;
  for (const auto& watchlist : watchlists) {
    array.emplace_back(MakeWatchlistSummary(watchlist));
  }
  return array;
}

auto MakeHoldingArray(const std::vector<application::HoldingRow>& holdings)
    -> json::array {
  json::array array;
  for (const auto& holding : holdings) {
    array.emplace_back(json::object{{"id", holding.id},
                                    {"symbol", holding.symbol},
                                    {"name", holding.name},
                                    {"quantity", holding.quantity},
                                    {"marketValue", MakeMoney(holding.market_value)},
                                    {"dayChange",
                                     MakeAbsolutePercentChange(holding.day_change)}});
  }
  return array;
}

auto MakeWatchlistRowArray(const std::vector<application::WatchlistRow>& rows)
    -> json::array {
  json::array array;
  for (const auto& row : rows) {
    array.emplace_back(json::object{{"id", row.id},
                                    {"symbol", row.symbol},
                                    {"name", row.name},
                                    {"lastPrice", MakeMoney(row.last_price)},
                                    {"dayChange",
                                     MakeAbsolutePercentChange(row.day_change)},
                                    {"bid", MakeMoney(row.bid)},
                                    {"ask", MakeMoney(row.ask)},
                                    {"updatedAt", row.updated_at}});
  }
  return array;
}

auto MakeAccountPositionArray(
    const std::vector<application::AccountPositionRow>& positions) -> json::array {
  json::array array;
  for (const auto& position : positions) {
    array.emplace_back(json::object{{"symbol", position.symbol},
                                    {"name", position.name},
                                    {"dayProfitLoss", MakeMoney(position.day_profit_loss)},
                                    {"dayProfitLossPercent",
                                     position.day_profit_loss_percent},
                                    {"averagePrice", MakeMoney(position.average_price)},
                                    {"marketValue", MakeMoney(position.market_value)},
                                    {"quantity", position.quantity}});
  }
  return array;
}

auto MakeInstrument(const application::Instrument& instrument) -> json::object {
  return {
      {"symbol", instrument.symbol},
      {"name", instrument.name},
      {"assetType", instrument.asset_type},
      {"primaryExchange", instrument.primary_exchange},
  };
}

auto MakeQuoteSnapshot(const application::QuoteSnapshot& quote) -> json::object {
  return {
      {"lastPrice", MakeMoney(quote.last_price)},
      {"bid", MakeMoney(quote.bid)},
      {"ask", MakeMoney(quote.ask)},
      {"open", MakeMoney(quote.open)},
      {"high", MakeMoney(quote.high)},
      {"low", MakeMoney(quote.low)},
      {"previousClose", MakeMoney(quote.previous_close)},
      {"volume", quote.volume},
      {"updatedAt", quote.updated_at},
  };
}

auto MakePositionSummary(const application::PositionSummary& position)
    -> json::object {
  return {
      {"quantity", position.quantity},
      {"marketValue", MakeMoney(position.market_value)},
      {"costBasis", MakeMoney(position.cost_basis)},
      {"unrealizedGain", MakeMoney(position.unrealized_gain)},
  };
}

auto MakeStringArray(const std::vector<std::string>& strings) -> json::array {
  json::array array;
  for (const auto& entry : strings) {
    array.emplace_back(entry);
  }
  return array;
}

auto MakeWatchlistMembership(
    const application::WatchlistMembership& membership) -> json::object {
  return {
      {"isSaved", membership.is_saved},
      {"watchlistIds", MakeStringArray(membership.watchlist_ids)},
  };
}

auto MakeChartSeries(const application::ChartSeries& series) -> json::object {
  json::array bars;
  for (const auto& bar : series.bars) {
    bars.emplace_back(json::object{{"time", bar.time},
                                   {"open", bar.open},
                                   {"high", bar.high},
                                   {"low", bar.low},
                                   {"close", bar.close},
                                   {"volume", bar.volume}});
  }

  return {
      {"type", series.type},
      {"bars", bars},
  };
}

auto MakeOptionExpirationArray(
    const std::vector<application::OptionExpirationSnapshot>& expirations)
    -> json::array {
  json::array array;
  for (const auto& expiration : expirations) {
    json::array rows;
    for (const auto& row : expiration.rows) {
      rows.emplace_back(json::object{{"id", row.id},
                                     {"strike", row.strike},
                                     {"callBid", row.call_bid},
                                     {"callAsk", row.call_ask},
                                     {"callLast", row.call_last},
                                     {"callChange", row.call_change},
                                     {"callDelta", row.call_delta},
                                     {"callGamma", row.call_gamma},
                                     {"callTheta", row.call_theta},
                                     {"callVega", row.call_vega},
                                     {"callOpenInterest", row.call_open_interest},
                                     {"putBid", row.put_bid},
                                     {"putAsk", row.put_ask},
                                     {"putLast", row.put_last},
                                     {"putChange", row.put_change},
                                     {"putDelta", row.put_delta},
                                     {"putGamma", row.put_gamma},
                                     {"putTheta", row.put_theta},
                                     {"putVega", row.put_vega},
                                     {"putOpenInterest", row.put_open_interest}});
    }
    array.emplace_back(json::object{{"id", expiration.id},
                                    {"label", expiration.label},
                                    {"gammaAtExpiry", expiration.gamma_at_expiry},
                                    {"rows", rows}});
  }
  return array;
}

auto WriteEnvelope(const json::value& data) -> std::string {
  return json::serialize(json::object{{"data", data}, {"meta", MakeMeta()}});
}

}  // namespace

auto SerializeHealthResponse() -> std::string {
  return WriteEnvelope(json::object{{"status", "ok"},
                                    {"service", "premia_api"},
                                    {"contract", "contracts/openapi/premia-v1.yaml"}});
}

auto SerializeBootstrapResponse(const application::BootstrapData& data)
    -> std::string {
  return WriteEnvelope(json::object{{"session",
                                     json::object{{"environment", data.environment},
                                                  {"featureFlags",
                                                   MakeCapabilities(data.feature_flags)}}},
                                    {"connections",
                                     MakeConnectionSummaryArray(data.connections)}});
}

auto SerializeHomeScreenResponse(const application::HomeScreenData& data)
    -> std::string {
  return WriteEnvelope(json::object{{"connections",
                                     MakeConnectionSummaryArray(data.connections)},
                                    {"portfolio",
                                     json::object{{"totalValue",
                                                   MakeMoney(data.portfolio.total_value)},
                                                  {"dayChange",
                                                   MakeAbsolutePercentChange(
                                                       data.portfolio.day_change)},
                                                  {"cash", MakeMoney(data.portfolio.cash)},
                                                  {"buyingPower",
                                                   MakeMoney(data.portfolio.buying_power)},
                                                  {"holdingsCount",
                                                   data.portfolio.holdings_count}}},
                                    {"topHoldings", MakeHoldingArray(data.top_holdings)},
                                    {"watchlists",
                                     MakeWatchlistSummaryArray(data.watchlists)},
                                    {"market",
                                     json::object{{"session", data.market.session},
                                                  {"nextTransitionAt",
                                                   data.market.next_transition_at}}}});
}

auto SerializeAccountScreenResponse(const application::AccountDetail& data)
    -> std::string {
  return WriteEnvelope(json::object{{"accountId", data.account_id},
                                    {"cash", MakeMoney(data.cash)},
                                    {"netLiquidation",
                                     MakeMoney(data.net_liquidation)},
                                    {"availableFunds",
                                     MakeMoney(data.available_funds)},
                                    {"longMarketValue",
                                     MakeMoney(data.long_market_value)},
                                    {"shortMarketValue",
                                     MakeMoney(data.short_market_value)},
                                    {"buyingPower", MakeMoney(data.buying_power)},
                                    {"equity", MakeMoney(data.equity)},
                                    {"equityPercentage", data.equity_percentage},
                                    {"marginBalance",
                                     MakeMoney(data.margin_balance)},
                                    {"positions",
                                     MakeAccountPositionArray(data.positions)}});
}

auto SerializeWatchlistsResponse(
    const std::vector<application::WatchlistSummary>& watchlists)
    -> std::string {
  return WriteEnvelope(
      json::object{{"watchlists", MakeWatchlistSummaryArray(watchlists)}});
}

auto SerializeWatchlistScreenResponse(const application::WatchlistScreenData& data)
    -> std::string {
  return WriteEnvelope(json::object{{"watchlist", MakeWatchlistSummary(data.watchlist)},
                                    {"availableWatchlists",
                                     MakeWatchlistSummaryArray(data.available_watchlists)},
                                    {"rows", MakeWatchlistRowArray(data.rows)}});
}

auto SerializeWatchlistResponse(const application::WatchlistSummary& data)
    -> std::string {
  return WriteEnvelope(MakeWatchlistSummary(data));
}

auto SerializeQuoteScreenResponse(const application::QuoteDetail& data)
    -> std::string {
  return WriteEnvelope(json::object{{"instrument", MakeInstrument(data.instrument)},
                                    {"quote", MakeQuoteSnapshot(data.quote)},
                                    {"position", MakePositionSummary(data.position)},
                                    {"watchlistMembership",
                                     MakeWatchlistMembership(
                                         data.watchlist_membership)}});
}

auto SerializeChartScreenResponse(const application::ChartScreenData& data)
    -> std::string {
  return WriteEnvelope(json::object{{"instrument", MakeInstrument(data.instrument)},
                                    {"range", data.range},
                                    {"interval", data.interval},
                                    {"timezone", data.timezone},
                                    {"series", MakeChartSeries(data.series)},
                                    {"stats",
                                     json::object{{"change",
                                                   MakeAbsolutePercentChange(
                                                       data.stats.change)}}}});
}

auto SerializeOptionChainResponse(const application::OptionChainSnapshot& data)
    -> std::string {
  return WriteEnvelope(json::object{{"symbol", data.symbol},
                                    {"description", data.description},
                                    {"bid", data.bid},
                                    {"ask", data.ask},
                                    {"openPrice", data.open_price},
                                    {"closePrice", data.close_price},
                                    {"highPrice", data.high_price},
                                    {"lowPrice", data.low_price},
                                    {"totalVolume", data.total_volume},
                                    {"volatility", data.volatility},
                                    {"gammaExposure", data.gamma_exposure},
                                    {"expirations",
                                     MakeOptionExpirationArray(data.expirations)}});
}

auto SerializeOrderPreviewResponse(const application::OrderPreviewData& data)
    -> std::string {
  return WriteEnvelope(json::object{{"previewId", data.preview_id},
                                    {"accountId", data.account_id},
                                    {"symbol", data.symbol},
                                    {"assetType", data.asset_type},
                                    {"instruction", data.instruction},
                                    {"quantity", data.quantity},
                                    {"orderType", data.order_type},
                                    {"limitPrice", data.limit_price},
                                    {"estimatedTotal", data.estimated_total},
                                    {"mode", data.mode},
                                    {"status", data.status},
                                    {"warnings", MakeStringArray(data.warnings)}});
}

auto SerializeOrderSubmissionResponse(
    const application::OrderSubmissionData& data) -> std::string {
  return WriteEnvelope(json::object{{"submissionId", data.submission_id},
                                    {"accountId", data.account_id},
                                    {"symbol", data.symbol},
                                    {"assetType", data.asset_type},
                                    {"instruction", data.instruction},
                                    {"quantity", data.quantity},
                                    {"orderType", data.order_type},
                                    {"limitPrice", data.limit_price},
                                    {"mode", data.mode},
                                    {"status", data.status},
                                    {"submittedAt", data.submitted_at},
                                    {"message", data.message}});
}

auto SerializeOrderCancellationResponse(
    const application::OrderCancellationData& data) -> std::string {
  return WriteEnvelope(json::object{{"orderId", data.order_id},
                                    {"accountId", data.account_id},
                                    {"mode", data.mode},
                                    {"status", data.status},
                                    {"cancelledAt", data.cancelled_at},
                                    {"message", data.message}});
}

auto SerializeOrderReplacementResponse(
    const application::OrderReplacementData& data) -> std::string {
  return WriteEnvelope(json::object{{"replacementId", data.replacement_id},
                                    {"replacedOrderId", data.replaced_order_id},
                                    {"accountId", data.account_id},
                                    {"symbol", data.symbol},
                                    {"assetType", data.asset_type},
                                    {"instruction", data.instruction},
                                    {"quantity", data.quantity},
                                    {"orderType", data.order_type},
                                    {"limitPrice", data.limit_price},
                                    {"mode", data.mode},
                                    {"status", data.status},
                                    {"submittedAt", data.submitted_at},
                                    {"message", data.message}});
}

auto SerializeOrderRecordsResponse(
    const std::vector<application::OrderRecordData>& data) -> std::string {
  json::array records;
  for (const auto& record : data) {
    records.emplace_back(json::object{{"orderId", record.order_id},
                                      {"accountId", record.account_id},
                                      {"symbol", record.symbol},
                                      {"assetType", record.asset_type},
                                      {"instruction", record.instruction},
                                      {"quantity", record.quantity},
                                      {"orderType", record.order_type},
                                      {"limitPrice", record.limit_price},
                                      {"mode", record.mode},
                                      {"status", record.status},
                                      {"submittedAt", record.submitted_at},
                                      {"updatedAt", record.updated_at},
                                      {"message", record.message}});
  }
  return WriteEnvelope(json::object{{"orders", records}});
}

auto SerializeConnectionSummaryResponse(const application::ConnectionSummary& data)
    -> std::string {
  return WriteEnvelope(MakeConnectionSummary(data));
}

auto SerializeSchwabOAuthStartResponse(
    const application::SchwabOAuthStartData& data) -> std::string {
  return WriteEnvelope(json::object{{"authUrl", data.auth_url},
                                    {"state", data.state},
                                    {"expiresAt", data.expires_at}});
}

auto SerializePlaidLinkTokenResponse(
    const application::PlaidLinkTokenData& data) -> std::string {
  return WriteEnvelope(json::object{{"linkToken", data.link_token},
                                    {"expiration", data.expiration}});
}

auto SerializeErrorResponse(const std::string& code, const std::string& message,
                            const std::string& action,
                            const std::string& provider) -> std::string {
  json::object error{{"code", code},
                     {"message", message},
                     {"retryable", false},
                     {"action", action}};
  if (!provider.empty()) {
    error["provider"] = provider;
  }

  return json::serialize(
      json::object{{"error", error}, {"meta", MakeErrorMeta()}});
}

}  // namespace premia::api::http

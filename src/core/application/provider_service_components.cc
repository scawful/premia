#include "provider_service_components.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Plaid/client.h"
#include "Schwab/client.h"
#include "premia/providers/local/account_detail_provider.hpp"
#include "premia/providers/local/options_provider.hpp"
#include "premia/providers/local/order_provider.hpp"
#include "premia/providers/local/portfolio_provider.hpp"
#include "premia/providers/local/watchlist_provider.hpp"
#include "premia/providers/plaid/workflow_provider.hpp"
#include "premia/providers/schwab/market_data_provider.hpp"
#include "premia/providers/schwab/workflow_provider.hpp"
#include "premia/providers/tda/account_detail_provider.hpp"
#include "premia/providers/tda/order_provider.hpp"
#include "premia/providers/tda/options_provider.hpp"
#include "premia/providers/tda/portfolio_provider.hpp"
#include "premia/providers/tda/watchlist_provider.hpp"

namespace premia::core::application::detail {

namespace domain = premia::core::domain;

namespace {

using domain::AbsolutePercentChange;
using domain::ConnectionStatus;
using domain::Money;
using domain::Provider;
namespace pt = boost::property_tree;

constexpr char kTDAConfigPath[] = "assets/tda.json";
constexpr char kSchwabConfigPath[] = "assets/schwab.json";
constexpr char kSchwabTokenPath[] = "assets/schwab_tokens.json";
constexpr char kPlaidConfigPath[] = "assets/plaid.json";
constexpr char kPlaidTokenPath[] = "assets/plaid_tokens.json";
constexpr char kWatchlistsPath[] = "assets/watchlists.json";
constexpr char kPortfolioPath[] = "assets/portfolio.json";
constexpr char kAccountPath[] = "assets/account.json";
constexpr char kOptionsPath[] = "assets/options.json";
constexpr char kOrdersPath[] = "assets/orders.json";

auto MakeMoney(std::string amount) -> Money {
  return Money{std::move(amount), "USD"};
}

auto MakeChange(std::string absolute, std::string percent)
    -> AbsolutePercentChange {
  return AbsolutePercentChange{MakeMoney(std::move(absolute)), std::move(percent)};
}

auto MakeConnection(Provider provider, ConnectionStatus status,
                    std::string display_name, std::string last_sync_at,
                    bool reauth_required,
                    std::map<std::string, bool> capabilities)
    -> ConnectionSummary {
  ConnectionSummary summary;
  summary.provider = provider;
  summary.status = status;
  summary.display_name = std::move(display_name);
  summary.last_sync_at = std::move(last_sync_at);
  summary.reauth_required = reauth_required;
  summary.capabilities = std::move(capabilities);
  return summary;
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

auto HasFile(const std::string& path) -> bool {
  std::ifstream file(path);
  return file.good();
}

auto HasUsableSchwabConfig() -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(kSchwabConfigPath, tree)) {
    return false;
  }
  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto HasUsablePlaidConfig() -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(kPlaidConfigPath, tree)) {
    return false;
  }
  const auto client_id = tree.get<std::string>("client_id", "");
  const auto secret = tree.get<std::string>("secret", "");
  return !IsPlaceholderValue(client_id) && !IsPlaceholderValue(secret);
}

auto LoadSchwabClient(premia::schwab::Client& client) -> bool {
  if (!HasUsableSchwabConfig()) {
    return false;
  }
  if (!client.LoadConfig(kSchwabConfigPath)) {
    return false;
  }
  client.LoadTokens(kSchwabTokenPath);
  return true;
}

auto LoadPlaidClient(premia::plaid::Client& client) -> bool {
  if (!HasUsablePlaidConfig()) {
    return false;
  }
  if (!client.LoadConfig(kPlaidConfigPath)) {
    return false;
  }
  client.LoadTokens(kPlaidTokenPath);
  return true;
}

auto BuildQuoteFallback(const std::string& symbol) -> QuoteDetail {
  QuoteDetail detail;
  detail.instrument = {symbol, symbol + " Holdings Demo", "equity", "NASDAQ"};
  detail.quote = QuoteSnapshot{MakeMoney("217.00"), MakeMoney("216.95"),
                               MakeMoney("217.02"), MakeMoney("214.10"),
                               MakeMoney("218.22"), MakeMoney("213.90"),
                               MakeMoney("215.70"), "53422010",
                               "2026-03-22T18:44:58Z"};
  detail.position = PositionSummary{"42", MakeMoney("9114.00"),
                                    MakeMoney("8421.00"), MakeMoney("693.00")};
  detail.watchlist_membership = WatchlistMembership{true, {"core"}};
  return detail;
}

auto BuildChartFallback(const std::string& symbol, const std::string& range,
                        const std::string& interval, bool extended_hours)
    -> ChartScreenData {
  ChartScreenData fallback;
  fallback.instrument = {symbol, symbol + " Holdings Demo", "equity", "NASDAQ"};
  fallback.range = range.empty() ? "1M" : range;
  fallback.interval = interval.empty() ? "1D" : interval;
  fallback.timezone = "America/New_York";
  fallback.series.type = extended_hours ? "line" : "candles";
  fallback.series.bars = {
      {"2026-03-18T20:00:00Z", "212.40", "214.60", "211.80", "213.90",
       "48221001"},
      {"2026-03-19T20:00:00Z", "213.95", "216.40", "213.10", "215.70",
       "50110221"},
      {"2026-03-20T20:00:00Z", "214.10", "218.22", "213.90", "217.00",
       "53422010"},
  };
  fallback.stats = ChartStats{MakeChange("8.20", "3.93")};
  return fallback;
}

}  // namespace

ConnectionService::ConnectionService() {
  connections_ = {
      MakeConnection(Provider::kSchwab, ConnectionStatus::kNotConnected,
                     "Charles Schwab", "", false,
                     {{"portfolio", true},
                      {"marketData", true},
                      {"options", true},
                      {"banking", false}}),
      MakeConnection(Provider::kPlaid, ConnectionStatus::kNotConnected, "Plaid",
                     "", false,
                     {{"banking", true},
                      {"transactions", true},
                      {"portfolio", false}}),
      MakeConnection(Provider::kIBKR, ConnectionStatus::kDegraded,
                     "Interactive Brokers", "2026-03-22T18:31:00Z", false,
                     {{"portfolio", true},
                      {"marketData", false},
                      {"trading", false}}),
  };
}

auto ConnectionService::GetConnections() const -> std::vector<ConnectionSummary> {
  auto connections = connections_;
  for (auto& connection : connections) {
    if (connection.provider == Provider::kSchwab) {
      premia::schwab::Client client;
      if (LoadSchwabClient(client)) {
        if (client.HasValidAccessToken()) {
          connection.status = ConnectionStatus::kConnected;
          connection.reauth_required = false;
        } else if (client.HasValidRefreshToken()) {
          connection.status = ConnectionStatus::kDegraded;
          connection.reauth_required = false;
        } else if (HasFile(kSchwabTokenPath)) {
          connection.status = ConnectionStatus::kReauthRequired;
          connection.reauth_required = true;
        } else {
          connection.status = ConnectionStatus::kNotConnected;
          connection.reauth_required = false;
          connection.last_sync_at.clear();
        }
      }
    }

    if (connection.provider == Provider::kPlaid) {
      premia::plaid::Client client;
      if (LoadPlaidClient(client)) {
        connection.status = client.HasAccessToken() ? ConnectionStatus::kConnected
                                                    : ConnectionStatus::kNotConnected;
        connection.reauth_required = false;
        if (!client.HasAccessToken()) {
          connection.last_sync_at.clear();
        }
      }
    }
  }
  return connections;
}

auto ConnectionService::GetConnection(const std::string& provider_key) const
    -> ConnectionSummary {
  const auto provider = domain::ProviderFromString(provider_key);
  const auto connections = GetConnections();
  auto it = std::find_if(connections.begin(), connections.end(),
                         [provider](const ConnectionSummary& connection) {
                           return connection.provider == provider;
                         });
  if (it == connections.end()) {
    throw std::out_of_range("provider connection not found");
  }
  return *it;
}

auto ConnectionService::MutableConnection(Provider provider) -> ConnectionSummary& {
  auto it = std::find_if(connections_.begin(), connections_.end(),
                         [provider](const ConnectionSummary& connection) {
                           return connection.provider == provider;
                         });
  if (it == connections_.end()) {
    throw std::out_of_range("provider connection not found");
  }
  return *it;
}

auto PortfolioAccountService::GetPortfolioSummary() const -> PortfolioSummary {
  try {
    providers::tda::PortfolioProvider provider(kTDAConfigPath);
    return provider.GetPortfolioSummary();
  } catch (const std::exception&) {
  }
  providers::local::PortfolioProvider provider(kPortfolioPath);
  return provider.GetPortfolioSummary();
}

auto PortfolioAccountService::GetTopHoldings() const -> std::vector<HoldingRow> {
  try {
    providers::tda::PortfolioProvider provider(kTDAConfigPath);
    return provider.GetTopHoldings();
  } catch (const std::exception&) {
  }
  providers::local::PortfolioProvider provider(kPortfolioPath);
  return provider.GetTopHoldings();
}

auto PortfolioAccountService::GetAccountDetail() const -> AccountDetail {
  try {
    providers::tda::AccountDetailProvider provider(kTDAConfigPath);
    return provider.GetAccountDetail();
  } catch (const std::exception&) {
  }
  providers::local::AccountDetailProvider provider(kAccountPath);
  return provider.GetAccountDetail();
}

auto MarketOptionsService::GetQuoteDetail(const std::string& symbol) const
    -> QuoteDetail {
  const auto fallback = BuildQuoteFallback(symbol);
  try {
    providers::schwab::MarketDataProvider provider(kSchwabConfigPath,
                                                   kSchwabTokenPath);
    auto detail = provider.GetQuoteDetail(symbol);
    detail.position = fallback.position;
    detail.watchlist_membership = fallback.watchlist_membership;
    if (detail.instrument.name.empty()) {
      detail.instrument.name = fallback.instrument.name;
    }
    if (detail.instrument.primary_exchange.empty()) {
      detail.instrument.primary_exchange = fallback.instrument.primary_exchange;
    }
    return detail;
  } catch (const std::exception&) {
    return fallback;
  }
}

auto MarketOptionsService::GetChartScreen(const std::string& symbol,
                                          const std::string& range,
                                          const std::string& interval,
                                          bool extended_hours) const
    -> ChartScreenData {
  const auto fallback = BuildChartFallback(symbol, range, interval, extended_hours);
  try {
    providers::schwab::MarketDataProvider provider(kSchwabConfigPath,
                                                   kSchwabTokenPath);
    auto chart = provider.GetChartScreen(symbol, fallback.range, fallback.interval,
                                         extended_hours);
    if (chart.instrument.name.empty()) {
      chart.instrument.name = fallback.instrument.name;
    }
    if (chart.series.bars.empty()) {
      return fallback;
    }
    return chart;
  } catch (const std::exception&) {
    return fallback;
  }
}

auto MarketOptionsService::GetOptionChainSnapshot(
    const std::string& symbol, const std::string& strike_count,
    const std::string& strategy, const std::string& range,
    const std::string& exp_month, const std::string& option_type) const
    -> OptionChainSnapshot {
  try {
    providers::tda::OptionsProvider provider(kTDAConfigPath);
    return provider.GetOptionChainSnapshot(symbol, strike_count, strategy, range,
                                           exp_month, option_type);
  } catch (const std::exception&) {
  }
  providers::local::OptionsProvider provider(kOptionsPath);
  return provider.GetOptionChainSnapshot(symbol, strike_count, strategy, range,
                                         exp_month, option_type);
}

auto WatchlistService::ListWatchlists() const -> std::vector<WatchlistSummary> {
  try {
    providers::tda::WatchlistProvider provider(kTDAConfigPath);
    return provider.ListWatchlists();
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.ListWatchlists();
}

auto WatchlistService::GetWatchlistScreen(const std::string& watchlist_id) const
    -> WatchlistScreenData {
  try {
    providers::tda::WatchlistProvider provider(kTDAConfigPath);
    return provider.GetWatchlistScreen(watchlist_id);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.GetWatchlistScreen(watchlist_id);
}

auto WatchlistService::CreateWatchlist(const std::string& name)
    -> WatchlistSummary {
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.CreateWatchlist(name);
}

auto WatchlistService::RenameWatchlist(const std::string& watchlist_id,
                                       const std::string& name)
    -> WatchlistSummary {
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.RenameWatchlist(watchlist_id, name);
}

auto WatchlistService::AddWatchlistSymbol(const std::string& watchlist_id,
                                          const std::string& symbol)
    -> WatchlistSummary {
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.AddWatchlistSymbol(watchlist_id, symbol);
}

auto WatchlistService::RemoveWatchlistSymbol(const std::string& watchlist_id,
                                             const std::string& symbol)
    -> WatchlistSummary {
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.RemoveWatchlistSymbol(watchlist_id, symbol);
}

auto OrderService::PreviewOrder(const OrderIntentRequest& request)
    -> OrderPreviewData {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.PreviewOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.PreviewOrder(request);
}

auto OrderService::SubmitOrder(const OrderIntentRequest& request)
    -> OrderSubmissionData {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.SubmitOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.SubmitOrder(request);
}

auto OrderService::CancelOrder(const OrderCancelRequest& request)
    -> OrderCancellationData {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.CancelOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.CancelOrder(request);
}

auto OrderService::ReplaceOrder(const OrderReplaceRequest& request)
    -> OrderReplacementData {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.ReplaceOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.ReplaceOrder(request);
}

auto OrderService::GetOpenOrders(const std::string& account_id) const
    -> std::vector<OrderRecordData> {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.GetOpenOrders(account_id);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.GetOpenOrders(account_id);
}

auto OrderService::GetOrderHistory(const std::string& account_id) const
    -> std::vector<OrderRecordData> {
  try {
    providers::tda::OrderProvider provider(kTDAConfigPath);
    return provider.GetOrderHistory(account_id);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.GetOrderHistory(account_id);
}

WorkflowService::WorkflowService(ConnectionService& connection_service)
    : connection_service_(connection_service) {}

auto WorkflowService::CreateLinkToken(const PlaidLinkTokenRequest& request)
    -> PlaidLinkTokenData {
  return CreatePlaidLinkToken(request);
}

auto WorkflowService::StartSchwabOAuth(const SchwabOAuthStartRequest& request)
    -> SchwabOAuthStartData {
  auto& schwab = connection_service_.MutableConnection(Provider::kSchwab);
  schwab.status = ConnectionStatus::kConnecting;
  schwab.reauth_required = false;

  const auto state = std::string("schwab_state_") + std::to_string(NextWorkflowId());
  providers::schwab::WorkflowProvider provider(kSchwabConfigPath,
                                               kSchwabTokenPath);
  return provider.StartOAuth(request, state, CurrentUtcTimestamp());
}

auto WorkflowService::CompleteSchwabOAuth(
    const SchwabOAuthCompleteRequest& request) -> ConnectionSummary {
  auto& schwab = connection_service_.MutableConnection(Provider::kSchwab);
  providers::schwab::WorkflowProvider provider(kSchwabConfigPath,
                                               kSchwabTokenPath);
  schwab = provider.CompleteOAuth(request, schwab, CurrentUtcTimestamp());
  return schwab;
}

auto WorkflowService::CreatePlaidLinkToken(const PlaidLinkTokenRequest& request)
    -> PlaidLinkTokenData {
  auto& plaid = connection_service_.MutableConnection(Provider::kPlaid);
  plaid.status = ConnectionStatus::kConnecting;
  providers::plaid::WorkflowProvider provider(kPlaidConfigPath, kPlaidTokenPath);
  return provider.CreateLinkToken(request, CurrentUtcTimestamp(), NextWorkflowId());
}

auto WorkflowService::CompletePlaidLink(
    const PlaidLinkCompleteRequest& request) -> ConnectionSummary {
  auto& plaid = connection_service_.MutableConnection(Provider::kPlaid);
  providers::plaid::WorkflowProvider provider(kPlaidConfigPath, kPlaidTokenPath);
  plaid = provider.CompleteLink(request, plaid, CurrentUtcTimestamp());
  return plaid;
}

auto WorkflowService::NextWorkflowId() -> unsigned long long {
  ++workflow_counter_;
  return workflow_counter_;
}

auto WorkflowService::CurrentUtcTimestamp() const -> std::string {
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

}  // namespace premia::core::application::detail

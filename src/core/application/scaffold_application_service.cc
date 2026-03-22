#include "premia/core/application/scaffold_application_service.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <stdexcept>

namespace premia::core::application {

namespace domain = premia::core::domain;

namespace {

using domain::AbsolutePercentChange;
using domain::ConnectionStatus;
using domain::Money;
using domain::Provider;

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

}  // namespace

auto ScaffoldApplicationService::Instance() -> ScaffoldApplicationService& {
  static ScaffoldApplicationService instance;
  return instance;
}

ScaffoldApplicationService::ScaffoldApplicationService() {
  connections_ = {
      MakeConnection(Provider::kSchwab, ConnectionStatus::kConnected,
                     "Charles Schwab", "2026-03-22T18:40:00Z", false,
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

  watchlists_ = {
      {"core", "Core", 12},
      {"earnings", "Earnings", 8},
      {"macro", "Macro", 6},
  };

  holdings_ = {
      {"holding_aapl", "AAPL", "Apple Inc.", "42", MakeMoney("9114.00"),
       MakeChange("54.60", "0.60")},
      {"holding_msft", "MSFT", "Microsoft Corp.", "18",
       MakeMoney("7561.80"), MakeChange("41.22", "0.55")},
      {"holding_spy", "SPY", "SPDR S&P 500 ETF", "25", MakeMoney("12987.50"),
       MakeChange("92.50", "0.72")},
  };
}

auto ScaffoldApplicationService::GetBootstrapData() const -> BootstrapData {
  BootstrapData data;
  data.environment = "development";
  data.feature_flags = {
      {"options", false},
      {"banking", false},
      {"streaming", true},
      {"desktopMigration", true},
  };
  data.connections = GetConnections();
  return data;
}

auto ScaffoldApplicationService::GetHomeScreenData() const -> HomeScreenData {
  HomeScreenData data;
  data.connections = GetConnections();
  data.portfolio = GetPortfolioSummary();
  data.top_holdings = GetTopHoldings();
  data.watchlists = ListWatchlists();
  data.market = MarketSummary{"open", "2026-03-22T20:00:00Z"};
  return data;
}

auto ScaffoldApplicationService::GetConnections() const
    -> std::vector<ConnectionSummary> {
  return connections_;
}

auto ScaffoldApplicationService::GetConnection(const std::string& provider_key) const
    -> ConnectionSummary {
  return FindConnection(domain::ProviderFromString(provider_key));
}

auto ScaffoldApplicationService::GetPortfolioSummary() const -> PortfolioSummary {
  return PortfolioSummary{MakeMoney("128345.22"), MakeChange("842.13", "0.66"),
                          MakeMoney("14320.00"), MakeMoney("28640.00"), 12};
}

auto ScaffoldApplicationService::GetTopHoldings() const -> std::vector<HoldingRow> {
  return holdings_;
}

auto ScaffoldApplicationService::GetQuoteDetail(const std::string& symbol) const
    -> QuoteDetail {
  return BuildQuoteDetailForSymbol(symbol);
}

auto ScaffoldApplicationService::GetChartScreen(const std::string& symbol,
                                                const std::string& range,
                                                const std::string& interval,
                                                bool extended_hours) const
    -> ChartScreenData {
  ChartScreenData data;
  data.instrument = {symbol, symbol + " Holdings Demo", "equity", "NASDAQ"};
  data.range = range.empty() ? "1M" : range;
  data.interval = interval.empty() ? "1D" : interval;
  data.timezone = "America/New_York";
  data.series = BuildChartSeriesForSymbol(symbol, extended_hours);
  data.stats = ChartStats{MakeChange("8.20", "3.93")};
  return data;
}

auto ScaffoldApplicationService::ListWatchlists() const
    -> std::vector<WatchlistSummary> {
  return watchlists_;
}

auto ScaffoldApplicationService::GetWatchlistScreen(const std::string& watchlist_id) const
    -> WatchlistScreenData {
  WatchlistScreenData data;
  data.available_watchlists = watchlists_;
  data.watchlist = watchlists_.front();
  for (const auto& watchlist : watchlists_) {
    if (watchlist.id == watchlist_id) {
      data.watchlist = watchlist;
      break;
    }
  }

  data.rows = {
      {"row_aapl", "AAPL", "Apple Inc.", MakeMoney("217.00"),
       MakeChange("1.30", "0.60"), MakeMoney("216.95"),
       MakeMoney("217.02"), "2026-03-22T18:44:58Z"},
      {"row_msft", "MSFT", "Microsoft Corp.", MakeMoney("420.10"),
       MakeChange("2.15", "0.51"), MakeMoney("420.00"),
       MakeMoney("420.15"), "2026-03-22T18:44:59Z"},
      {"row_spy", "SPY", "SPDR S&P 500 ETF", MakeMoney("519.50"),
       MakeChange("3.70", "0.72"), MakeMoney("519.45"),
       MakeMoney("519.54"), "2026-03-22T18:45:00Z"},
  };
  return data;
}

auto ScaffoldApplicationService::CreateLinkToken(const PlaidLinkTokenRequest& request)
    -> PlaidLinkTokenData {
  return CreatePlaidLinkToken(request);
}

auto ScaffoldApplicationService::StartSchwabOAuth(
    const SchwabOAuthStartRequest& request) -> SchwabOAuthStartData {
  auto& schwab = FindConnection(Provider::kSchwab);
  schwab.status = ConnectionStatus::kConnecting;
  schwab.reauth_required = false;

  const auto state = std::string("schwab_state_") + std::to_string(NextWorkflowId());
  const auto redirect_uri = request.redirect_uri.empty()
                                ? std::string("premia://schwab/callback")
                                : request.redirect_uri;

  SchwabOAuthStartData data;
  data.state = state;
  data.expires_at = CurrentUtcTimestamp();
  data.auth_url =
      "https://api.schwabapi.com/v1/oauth/authorize?response_type=code&client_id="
      "premia-demo-app&redirect_uri=" +
      redirect_uri + "&state=" + state;
  return data;
}

auto ScaffoldApplicationService::CompleteSchwabOAuth(
    const SchwabOAuthCompleteRequest& request) -> ConnectionSummary {
  auto& schwab = FindConnection(Provider::kSchwab);
  if (!request.callback.empty()) {
    schwab.status = ConnectionStatus::kConnected;
    schwab.last_sync_at = CurrentUtcTimestamp();
    schwab.reauth_required = false;
  }
  return schwab;
}

auto ScaffoldApplicationService::CreatePlaidLinkToken(
    const PlaidLinkTokenRequest& request) -> PlaidLinkTokenData {
  auto& plaid = FindConnection(Provider::kPlaid);
  plaid.status = ConnectionStatus::kConnecting;

  PlaidLinkTokenData data;
  data.link_token = "link-sandbox-" + request.user_id + "-" +
                    std::to_string(NextWorkflowId());
  data.expiration = CurrentUtcTimestamp();
  return data;
}

auto ScaffoldApplicationService::CompletePlaidLink(
    const PlaidLinkCompleteRequest& request) -> ConnectionSummary {
  auto& plaid = FindConnection(Provider::kPlaid);
  if (!request.public_token.empty()) {
    plaid.status = ConnectionStatus::kConnected;
    plaid.last_sync_at = CurrentUtcTimestamp();
    plaid.reauth_required = false;
  }
  return plaid;
}

auto ScaffoldApplicationService::FindConnection(Provider provider)
    -> ConnectionSummary& {
  auto it = std::find_if(connections_.begin(), connections_.end(),
                         [provider](const ConnectionSummary& connection) {
                           return connection.provider == provider;
                         });
  if (it == connections_.end()) {
    throw std::out_of_range("provider connection not found");
  }
  return *it;
}

auto ScaffoldApplicationService::FindConnection(Provider provider) const
    -> const ConnectionSummary& {
  auto it = std::find_if(connections_.begin(), connections_.end(),
                         [provider](const ConnectionSummary& connection) {
                           return connection.provider == provider;
                         });
  if (it == connections_.end()) {
    throw std::out_of_range("provider connection not found");
  }
  return *it;
}

auto ScaffoldApplicationService::BuildQuoteDetailForSymbol(const std::string& symbol) const
    -> QuoteDetail {
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

auto ScaffoldApplicationService::BuildChartSeriesForSymbol(
    const std::string& symbol, bool extended_hours) const -> ChartSeries {
  (void)symbol;
  ChartSeries series;
  series.type = extended_hours ? "line" : "candles";
  series.bars = {
      {"2026-03-18T20:00:00Z", "212.40", "214.60", "211.80", "213.90",
       "48221001"},
      {"2026-03-19T20:00:00Z", "213.95", "216.40", "213.10", "215.70",
       "50110221"},
      {"2026-03-20T20:00:00Z", "214.10", "218.22", "213.90", "217.00",
       "53422010"},
  };
  return series;
}

auto ScaffoldApplicationService::NextWorkflowId() -> unsigned long long {
  ++workflow_counter_;
  return workflow_counter_;
}

auto ScaffoldApplicationService::CurrentUtcTimestamp() const -> std::string {
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

}  // namespace premia::core::application

#include "premia/core/application/scaffold_application_service.hpp"

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
#include "premia/providers/local/watchlist_provider.hpp"
#include "premia/providers/schwab/market_data_provider.hpp"

namespace premia::core::application {

namespace domain = premia::core::domain;

namespace {

using domain::AbsolutePercentChange;
using domain::ConnectionStatus;
using domain::Money;
using domain::Provider;
namespace pt = boost::property_tree;

constexpr char kSchwabConfigPath[] = "assets/schwab.json";
constexpr char kSchwabTokenPath[] = "assets/schwab_tokens.json";
constexpr char kPlaidConfigPath[] = "assets/plaid.json";
constexpr char kPlaidTokenPath[] = "assets/plaid_tokens.json";

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

auto ParsePlaidLinkTokenResponse(const std::string& response)
    -> PlaidLinkTokenData {
  PlaidLinkTokenData data;
  if (response.empty()) {
    return data;
  }

  try {
    std::istringstream ss(response);
    pt::ptree tree;
    pt::read_json(ss, tree);
    data.link_token = tree.get<std::string>("link_token", "");
    data.expiration = tree.get<std::string>("expiration", "");
  } catch (const std::exception&) {
  }

  return data;
}

}  // namespace

auto ScaffoldApplicationService::Instance() -> ScaffoldApplicationService& {
  static ScaffoldApplicationService instance;
  return instance;
}

ScaffoldApplicationService::ScaffoldApplicationService() {
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
        connection.status =
            client.HasAccessToken() ? ConnectionStatus::kConnected
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

auto ScaffoldApplicationService::GetConnection(const std::string& provider_key) const
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

auto ScaffoldApplicationService::GetPortfolioSummary() const -> PortfolioSummary {
  return PortfolioSummary{MakeMoney("128345.22"), MakeChange("842.13", "0.66"),
                          MakeMoney("14320.00"), MakeMoney("28640.00"), 12};
}

auto ScaffoldApplicationService::GetTopHoldings() const -> std::vector<HoldingRow> {
  return holdings_;
}

auto ScaffoldApplicationService::GetQuoteDetail(const std::string& symbol) const
    -> QuoteDetail {
  const auto fallback = BuildQuoteDetailForSymbol(symbol);
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

auto ScaffoldApplicationService::GetChartScreen(const std::string& symbol,
                                                const std::string& range,
                                                const std::string& interval,
                                                bool extended_hours) const
    -> ChartScreenData {
  ChartScreenData fallback;
  fallback.instrument = {symbol, symbol + " Holdings Demo", "equity", "NASDAQ"};
  fallback.range = range.empty() ? "1M" : range;
  fallback.interval = interval.empty() ? "1D" : interval;
  fallback.timezone = "America/New_York";
  fallback.series = BuildChartSeriesForSymbol(symbol, extended_hours);
  fallback.stats = ChartStats{MakeChange("8.20", "3.93")};

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

auto ScaffoldApplicationService::ListWatchlists() const
    -> std::vector<WatchlistSummary> {
  providers::local::WatchlistProvider provider("assets/watchlists.json");
  return provider.ListWatchlists();
}

auto ScaffoldApplicationService::GetWatchlistScreen(
    const std::string& watchlist_id) const -> WatchlistScreenData {
  providers::local::WatchlistProvider provider("assets/watchlists.json");
  return provider.GetWatchlistScreen(watchlist_id);
}

auto ScaffoldApplicationService::CreateLinkToken(
    const PlaidLinkTokenRequest& request) -> PlaidLinkTokenData {
  return CreatePlaidLinkToken(request);
}

auto ScaffoldApplicationService::StartSchwabOAuth(
    const SchwabOAuthStartRequest& request) -> SchwabOAuthStartData {
  auto& schwab = FindConnection(Provider::kSchwab);
  schwab.status = ConnectionStatus::kConnecting;
  schwab.reauth_required = false;

  const auto state = std::string("schwab_state_") + std::to_string(NextWorkflowId());

  SchwabOAuthStartData data;
  data.state = state;
  data.expires_at = CurrentUtcTimestamp();

  premia::schwab::Client client;
  if (LoadSchwabClient(client)) {
    data.auth_url = client.BuildAuthUrl();
  } else {
    const auto redirect_uri = request.redirect_uri.empty()
                                  ? std::string("premia://schwab/callback")
                                  : request.redirect_uri;
    data.auth_url =
        "https://api.schwabapi.com/v1/oauth/authorize?response_type=code&client_id="
        "premia-demo-app&redirect_uri=" +
        redirect_uri + "&state=" + state;
  }
  return data;
}

auto ScaffoldApplicationService::CompleteSchwabOAuth(
    const SchwabOAuthCompleteRequest& request) -> ConnectionSummary {
  auto& schwab = FindConnection(Provider::kSchwab);

  premia::schwab::Client client;
  if (LoadSchwabClient(client)) {
    if (!request.callback.empty() && client.ExchangeAuthCode(request.callback)) {
      client.SaveTokens(kSchwabTokenPath);
      client.GetAccountNumbers();
      schwab.status = ConnectionStatus::kConnected;
      schwab.last_sync_at = CurrentUtcTimestamp();
      schwab.reauth_required = false;
    } else if (!request.callback.empty()) {
      schwab.status = ConnectionStatus::kReauthRequired;
      schwab.reauth_required = true;
    }
    return schwab;
  }

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

  premia::plaid::Client client;
  if (LoadPlaidClient(client)) {
    const auto user_id = request.user_id.empty() ? std::string("premia-user")
                                                 : request.user_id;
    auto data = ParsePlaidLinkTokenResponse(client.CreateLinkToken(user_id));
    if (!data.link_token.empty()) {
      return data;
    }
  }

  PlaidLinkTokenData data;
  const auto user_id = request.user_id.empty() ? std::string("premia-user")
                                               : request.user_id;
  data.link_token = "link-sandbox-" + user_id + "-" +
                    std::to_string(NextWorkflowId());
  data.expiration = CurrentUtcTimestamp();
  return data;
}

auto ScaffoldApplicationService::CompletePlaidLink(
    const PlaidLinkCompleteRequest& request) -> ConnectionSummary {
  auto& plaid = FindConnection(Provider::kPlaid);

  premia::plaid::Client client;
  if (LoadPlaidClient(client)) {
    if (!request.public_token.empty() &&
        client.ExchangePublicToken(request.public_token)) {
      client.SaveTokens(kPlaidTokenPath);
      plaid.status = ConnectionStatus::kConnected;
      plaid.last_sync_at = CurrentUtcTimestamp();
      plaid.reauth_required = false;
    } else if (!request.public_token.empty()) {
      plaid.status = ConnectionStatus::kNotConnected;
      plaid.reauth_required = false;
      plaid.last_sync_at.clear();
    }
    return plaid;
  }

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

auto ScaffoldApplicationService::BuildQuoteDetailForSymbol(
    const std::string& symbol) const -> QuoteDetail {
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

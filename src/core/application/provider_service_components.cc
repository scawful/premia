#include "provider_service_components.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "premia/infrastructure/secrets/runtime_paths.hpp"
#include "premia/infrastructure/secrets/secret_store.hpp"
#include "Plaid/client.h"
#include "Schwab/client.h"
#include "premia/providers/local/account_detail_provider.hpp"
#include "premia/providers/local/options_provider.hpp"
#include "premia/providers/local/order_provider.hpp"
#include "premia/providers/local/portfolio_provider.hpp"
#include "premia/providers/local/watchlist_provider.hpp"
#include "premia/providers/plaid/workflow_provider.hpp"
#include "premia/providers/ibkr/account_detail_provider.hpp"
#include "premia/providers/schwab/account_detail_provider.hpp"
#include "premia/providers/schwab/market_data_provider.hpp"
#include "premia/providers/schwab/order_provider.hpp"
#include "premia/providers/schwab/options_provider.hpp"
#include "premia/providers/schwab/workflow_provider.hpp"
#include "premia/providers/tda/account_detail_provider.hpp"
#include "premia/providers/tda/order_provider.hpp"
#include "premia/providers/tda/options_provider.hpp"
#include "premia/providers/tda/portfolio_provider.hpp"
#include "premia/providers/tda/watchlist_provider.hpp"

namespace premia::core::application::detail {

namespace domain = premia::core::domain;
namespace secrets = premia::infrastructure::secrets;

namespace {

using domain::AbsolutePercentChange;
using domain::ConnectionStatus;
using domain::Money;
using domain::Provider;
namespace pt = boost::property_tree;

constexpr char kTDAConfigPath[] = "assets/tda.json";
constexpr char kIBKRConfigPath[] = "assets/ibkr.json";
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

auto ReadTextFile(const std::string& path) -> std::string {
  std::ifstream input(path, std::ios::binary);
  if (!input.good()) {
    return "";
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

auto HasFile(const std::string& path) -> bool {
  std::ifstream file(path);
  return file.good();
}

auto HasUsableTdaConfigAt(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(path, tree)) {
    return false;
  }
  const auto consumer_key = tree.get<std::string>("consumer_key", "");
  const auto refresh_token = tree.get<std::string>("refresh_token", "");
  return !IsPlaceholderValue(consumer_key) && !IsPlaceholderValue(refresh_token);
}

auto HasUsableIbkrConfigAt(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(path, tree)) {
    return false;
  }
  const auto host = tree.get<std::string>("host", "");
  const auto port = tree.get<int>("port", 0);
  return !host.empty() && port > 0;
}

auto HasUsableSchwabConfigAt(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(path, tree)) {
    return false;
  }
  const auto app_key = tree.get<std::string>("app_key", "");
  const auto app_secret = tree.get<std::string>("app_secret", "");
  return !IsPlaceholderValue(app_key) && !IsPlaceholderValue(app_secret);
}

auto HasUsablePlaidConfigAt(const std::string& path) -> bool {
  pt::ptree tree;
  if (!ReadJsonTree(path, tree)) {
    return false;
  }
  const auto client_id = tree.get<std::string>("client_id", "");
  const auto secret = tree.get<std::string>("secret", "");
  return !IsPlaceholderValue(client_id) && !IsPlaceholderValue(secret);
}

auto ResolveConfigPath(secrets::ProviderKind provider,
                       const std::string& legacy_path,
                       const std::function<bool(const std::string&)>& is_usable)
    -> std::string {
  const auto runtime_path = secrets::ProviderConfigPath(provider);
  const auto runtime_path_string = runtime_path.string();
  if (auto secret = secrets::LoadSecret(provider, secrets::SecretKind::kConfig)) {
    secrets::WriteSecureText(runtime_path, *secret);
    return runtime_path_string;
  }
  if (is_usable(runtime_path_string)) {
    if (secrets::KeychainEnabled()) {
      secrets::SaveSecret(provider, secrets::SecretKind::kConfig,
                          ReadTextFile(runtime_path_string));
    }
    return runtime_path_string;
  }
  if (is_usable(legacy_path)) {
    secrets::CopyFileToSecureStore(legacy_path, runtime_path);
    if (secrets::KeychainEnabled()) {
      secrets::SaveSecret(provider, secrets::SecretKind::kConfig,
                          ReadTextFile(legacy_path));
    }
    return runtime_path_string;
  }
  if (secrets::FileExists(runtime_path)) {
    return runtime_path_string;
  }
  return legacy_path;
}

auto ResolveTokenPath(secrets::ProviderKind provider,
                      const std::string& legacy_path) -> std::string {
  const auto runtime_path = secrets::ProviderTokenPath(provider);
  if (auto secret = secrets::LoadSecret(provider, secrets::SecretKind::kTokens)) {
    secrets::WriteSecureText(runtime_path, *secret);
    return runtime_path.string();
  }
  if (secrets::FileExists(runtime_path)) {
    if (secrets::KeychainEnabled()) {
      secrets::SaveSecret(provider, secrets::SecretKind::kTokens,
                          ReadTextFile(runtime_path.string()));
    }
    return runtime_path.string();
  }
  if (secrets::FileExists(legacy_path)) {
    secrets::CopyFileToSecureStore(legacy_path, runtime_path);
    if (secrets::KeychainEnabled()) {
      secrets::SaveSecret(provider, secrets::SecretKind::kTokens,
                          ReadTextFile(legacy_path));
    }
    return runtime_path.string();
  }
  secrets::EnsureProviderDir(provider);
  return runtime_path.string();
}

auto TdaConfigPath() -> std::string {
  return ResolveConfigPath(secrets::ProviderKind::kTDA, kTDAConfigPath,
                           HasUsableTdaConfigAt);
}

auto IbkrConfigPath() -> std::string {
  return ResolveConfigPath(secrets::ProviderKind::kIBKR, kIBKRConfigPath,
                           HasUsableIbkrConfigAt);
}

auto SchwabConfigPath() -> std::string {
  return ResolveConfigPath(secrets::ProviderKind::kSchwab, kSchwabConfigPath,
                           HasUsableSchwabConfigAt);
}

auto SchwabTokenPath() -> std::string {
  return ResolveTokenPath(secrets::ProviderKind::kSchwab, kSchwabTokenPath);
}

auto PlaidConfigPath() -> std::string {
  return ResolveConfigPath(secrets::ProviderKind::kPlaid, kPlaidConfigPath,
                           HasUsablePlaidConfigAt);
}

auto PlaidTokenPath() -> std::string {
  return ResolveTokenPath(secrets::ProviderKind::kPlaid, kPlaidTokenPath);
}

auto LoadSchwabClient(premia::schwab::Client& client) -> bool {
  const auto config_path = SchwabConfigPath();
  if (!HasUsableSchwabConfigAt(config_path)) {
    return false;
  }
  if (!client.LoadConfig(config_path)) {
    return false;
  }
  client.LoadTokens(SchwabTokenPath());
  return true;
}

auto LoadPlaidClient(premia::plaid::Client& client) -> bool {
  const auto config_path = PlaidConfigPath();
  if (!HasUsablePlaidConfigAt(config_path)) {
    return false;
  }
  if (!client.LoadConfig(config_path)) {
    return false;
  }
  client.LoadTokens(PlaidTokenPath());
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

auto MakePortfolioSummaryFromAccount(const AccountDetail& detail) -> PortfolioSummary {
  return PortfolioSummary{detail.net_liquidation,
                          {MakeMoney("0.00"), "0.00"},
                          detail.cash,
                          detail.buying_power,
                          static_cast<int>(detail.positions.size())};
}

auto ParsePriceString(const std::string& value) -> double {
  try {
    return std::stod(value);
  } catch (...) {
    return 0.0;
  }
}

auto LooksFilledStatus(std::string status) -> bool {
  std::transform(status.begin(), status.end(), status.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return status.find("fill") != std::string::npos ||
         status.find("execut") != std::string::npos ||
         status.find("complete") != std::string::npos;
}

auto ChartAnnotationStatePath() -> std::string {
  return (infrastructure::secrets::RuntimeRoot() / "chart_annotations.json").string();
}

auto LoadPersistedChartAnnotations(const std::string& account_id,
                                   const std::string& symbol)
    -> std::vector<ChartAnnotation> {
  std::ifstream input(ChartAnnotationStatePath());
  if (!input.good()) {
    return {};
  }

  std::vector<ChartAnnotation> annotations;
  try {
    pt::ptree tree;
    pt::read_json(input, tree);
    const auto key = (account_id.empty() ? std::string("default") : account_id) +
                     "|" + symbol;
    if (auto items = tree.get_child_optional("annotations." + key)) {
      for (const auto& item : *items) {
        annotations.push_back(ChartAnnotation{item.second.get<std::string>("id", ""),
                                             item.second.get<std::string>("label", ""),
                                             item.second.get<std::string>("price", "0.00"),
                                             item.second.get<std::string>("kind", "annotation")});
      }
    }
  } catch (const std::exception&) {
  }
  return annotations;
}

void SavePersistedChartAnnotations(const std::string& account_id,
                                   const std::string& symbol,
                                   const std::vector<ChartAnnotation>& annotations) {
  pt::ptree root;
  {
    std::ifstream input(ChartAnnotationStatePath());
    if (input.good()) {
      try {
        pt::read_json(input, root);
      } catch (const std::exception&) {
        root.clear();
      }
    }
  }

  const auto key = (account_id.empty() ? std::string("default") : account_id) +
                   "|" + symbol;
  pt::ptree items;
  for (const auto& annotation : annotations) {
    pt::ptree item;
    item.put("id", annotation.id);
    item.put("label", annotation.label);
    item.put("price", annotation.price);
    item.put("kind", annotation.kind);
    items.push_back({"", item});
  }
  root.put_child("annotations." + key, items);

  std::ofstream output(ChartAnnotationStatePath());
  pt::write_json(output, root);
}

auto BuildChartAnnotations(const std::string& symbol)
    -> std::vector<ChartAnnotation> {
  std::vector<ChartAnnotation> annotations;
  try {
    PortfolioAccountService portfolio_service;
    const auto account = portfolio_service.GetAccountDetail();
    annotations = LoadPersistedChartAnnotations(account.account_id, symbol);
    for (const auto& position : account.positions) {
      if (position.symbol != symbol) {
        continue;
      }
      annotations.push_back(ChartAnnotation{symbol + ":avg-cost", "Avg Cost",
                                            position.average_price.amount,
                                            "avg_cost"});
    }

    OrderService order_service;
    for (const auto& order : order_service.GetOpenOrders(account.account_id)) {
      if (order.symbol != symbol || order.limit_price.empty() ||
          ParsePriceString(order.limit_price) <= 0.0) {
        continue;
      }
      annotations.push_back(ChartAnnotation{symbol + ":order:" + order.order_id,
                                            order.instruction + " " + order.quantity,
                                            order.limit_price,
                                            "order"});
    }
    for (const auto& order : order_service.GetOrderHistory(account.account_id)) {
      if (order.symbol != symbol || order.limit_price.empty() ||
          ParsePriceString(order.limit_price) <= 0.0 ||
          !LooksFilledStatus(order.status)) {
        continue;
      }
      annotations.push_back(ChartAnnotation{symbol + ":fill:" + order.order_id,
                                            "Fill " + order.instruction + " " + order.quantity,
                                            order.limit_price,
                                            "fill"});
    }
  } catch (const std::exception&) {
  }
  return annotations;
}

auto MakeBrokerageAccountSummary(const AccountDetail& detail, Provider provider,
                                 const std::string& display_name)
    -> BrokerageAccountSummary {
  BrokerageAccountSummary summary;
  summary.provider = provider;
  summary.account_id = detail.account_id;
  summary.display_name = display_name;
  summary.total_value = detail.net_liquidation;
  summary.holdings_count = static_cast<int>(detail.positions.size());
  return summary;
}

}  // namespace

auto MakeHoldingsFromAccount(const AccountDetail& detail, Provider provider)
    -> std::vector<HoldingRow> {
  const auto provider_key = domain::ProviderToString(provider);
  std::vector<HoldingRow> holdings;
  holdings.reserve(detail.positions.size());
  for (const auto& position : detail.positions) {
    holdings.push_back({provider_key + ":" + position.symbol,
                        position.symbol,
                        position.name,
                        position.quantity,
                        position.market_value,
                        {position.day_profit_loss, position.day_profit_loss_percent}});
  }
  return holdings;
}

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
      MakeConnection(Provider::kIBKR, ConnectionStatus::kNotConnected,
                     "Interactive Brokers", "", false,
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
        } else if (HasFile(SchwabTokenPath())) {
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

    if (connection.provider == Provider::kIBKR) {
      const auto config_path = IbkrConfigPath();
      if (!HasUsableIbkrConfigAt(config_path)) {
        connection.status = ConnectionStatus::kNotConnected;
        connection.last_sync_at.clear();
      } else {
        try {
          providers::ibkr::AccountDetailProvider provider(config_path);
          (void)provider.GetAccountDetail();
          connection.status = ConnectionStatus::kConnected;
          connection.last_sync_at.clear();
        } catch (const std::exception&) {
          connection.status = ConnectionStatus::kDegraded;
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
  return GetPortfolioSummaryForAccount("");
}

auto PortfolioAccountService::ListBrokerageAccounts() const
    -> std::vector<BrokerageAccountSummary> {
  try {
    providers::schwab::AccountDetailProvider provider(SchwabConfigPath(),
                                                      SchwabTokenPath());
    auto accounts = provider.ListBrokerageAccounts();
    if (!accounts.empty()) {
      return accounts;
    }
  } catch (const std::exception&) {
  }

  try {
    providers::ibkr::AccountDetailProvider provider(IbkrConfigPath());
    const auto detail = provider.GetAccountDetail();
    return {MakeBrokerageAccountSummary(detail, Provider::kIBKR,
                                        "Interactive Brokers")};
  } catch (const std::exception&) {
  }

  try {
    providers::tda::AccountDetailProvider provider(TdaConfigPath());
    const auto detail = provider.GetAccountDetail();
    return {MakeBrokerageAccountSummary(detail, Provider::kTDA,
                                        "TDAmeritrade")};
  } catch (const std::exception&) {
  }

  providers::local::AccountDetailProvider provider(kAccountPath);
  const auto detail = provider.GetAccountDetail();
  return {MakeBrokerageAccountSummary(detail, Provider::kInternal,
                                      "Local Preview")};
}

auto PortfolioAccountService::GetPortfolioSummaryForAccount(
    const std::string& account_id) const -> PortfolioSummary {
  try {
    providers::schwab::AccountDetailProvider provider(SchwabConfigPath(),
                                                      SchwabTokenPath());
    return MakePortfolioSummaryFromAccount(
        account_id.empty() ? provider.GetAccountDetail()
                           : provider.GetAccountDetailForAccount(account_id));
  } catch (const std::exception&) {
  }
  try {
    providers::ibkr::AccountDetailProvider provider(IbkrConfigPath());
    const auto detail = provider.GetAccountDetail();
    if (account_id.empty() || detail.account_id == account_id) {
      return MakePortfolioSummaryFromAccount(detail);
    }
  } catch (const std::exception&) {
  }
  try {
    providers::tda::PortfolioProvider provider(TdaConfigPath());
    return provider.GetPortfolioSummary();
  } catch (const std::exception&) {
  }
  providers::local::PortfolioProvider provider(kPortfolioPath);
  return provider.GetPortfolioSummary();
}

auto PortfolioAccountService::GetTopHoldings() const -> std::vector<HoldingRow> {
  return GetTopHoldingsForAccount("");
}

auto PortfolioAccountService::GetTopHoldingsForAccount(
    const std::string& account_id) const -> std::vector<HoldingRow> {
  try {
    providers::schwab::AccountDetailProvider provider(SchwabConfigPath(),
                                                      SchwabTokenPath());
    return MakeHoldingsFromAccount(
        account_id.empty() ? provider.GetAccountDetail()
                           : provider.GetAccountDetailForAccount(account_id),
        Provider::kSchwab);
  } catch (const std::exception&) {
  }
  try {
    providers::ibkr::AccountDetailProvider provider(IbkrConfigPath());
    const auto detail = provider.GetAccountDetail();
    if (account_id.empty() || detail.account_id == account_id) {
      return MakeHoldingsFromAccount(detail, Provider::kIBKR);
    }
  } catch (const std::exception&) {
  }
  try {
    providers::tda::PortfolioProvider provider(TdaConfigPath());
    return provider.GetTopHoldings();
  } catch (const std::exception&) {
  }
  providers::local::PortfolioProvider provider(kPortfolioPath);
  return provider.GetTopHoldings();
}

auto PortfolioAccountService::GetAccountDetail() const -> AccountDetail {
  return GetAccountDetailForAccount("");
}

auto PortfolioAccountService::GetAccountDetailForAccount(
    const std::string& account_id) const -> AccountDetail {
  try {
    providers::schwab::AccountDetailProvider provider(SchwabConfigPath(),
                                                      SchwabTokenPath());
    return account_id.empty() ? provider.GetAccountDetail()
                              : provider.GetAccountDetailForAccount(account_id);
  } catch (const std::exception&) {
  }
  try {
    providers::ibkr::AccountDetailProvider provider(IbkrConfigPath());
    const auto detail = provider.GetAccountDetail();
    if (account_id.empty() || detail.account_id == account_id) {
      return detail;
    }
  } catch (const std::exception&) {
  }
  try {
    providers::tda::AccountDetailProvider provider(TdaConfigPath());
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
    providers::schwab::MarketDataProvider provider(SchwabConfigPath(),
                                                   SchwabTokenPath());
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
    providers::schwab::MarketDataProvider provider(SchwabConfigPath(),
                                                   SchwabTokenPath());
    auto chart = provider.GetChartScreen(symbol, fallback.range, fallback.interval,
                                         extended_hours);
    if (chart.instrument.name.empty()) {
      chart.instrument.name = fallback.instrument.name;
    }
    if (chart.series.bars.empty()) {
      auto fallback_chart = fallback;
      fallback_chart.annotations = BuildChartAnnotations(symbol);
      return fallback_chart;
    }
    chart.annotations = BuildChartAnnotations(symbol);
    return chart;
  } catch (const std::exception&) {
    auto fallback_chart = fallback;
    fallback_chart.annotations = BuildChartAnnotations(symbol);
    return fallback_chart;
  }
}

auto MarketOptionsService::ReplaceChartAnnotations(
    const std::string& symbol,
    const std::vector<ChartAnnotation>& annotations) -> ChartScreenData {
  PortfolioAccountService portfolio_service;
  const auto account = portfolio_service.GetAccountDetail();

  std::vector<ChartAnnotation> persisted;
  persisted.reserve(annotations.size());
  for (const auto& annotation : annotations) {
    if (annotation.kind == "annotation" || annotation.kind == "entry" ||
        annotation.kind == "stop" || annotation.kind == "target") {
      persisted.push_back(annotation);
    }
  }
  SavePersistedChartAnnotations(account.account_id, symbol, persisted);
  return GetChartScreen(symbol, "1M", "1D", false);
}

auto MarketOptionsService::GetOptionChainSnapshot(
    const std::string& symbol, const std::string& strike_count,
    const std::string& strategy, const std::string& range,
    const std::string& exp_month, const std::string& option_type) const
    -> OptionChainSnapshot {
  try {
    providers::schwab::OptionsProvider provider(SchwabConfigPath(),
                                                SchwabTokenPath());
    return provider.GetOptionChainSnapshot(symbol, strike_count, strategy, range,
                                           exp_month, option_type);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OptionsProvider provider(TdaConfigPath());
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
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.ListWatchlists();
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.ListWatchlists();
}

auto WatchlistService::GetWatchlistScreen(const std::string& watchlist_id) const
    -> WatchlistScreenData {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.GetWatchlistScreen(watchlist_id);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.GetWatchlistScreen(watchlist_id);
}

auto WatchlistService::CreateWatchlist(const std::string& name)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.CreateWatchlist(name);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.CreateWatchlist(name);
}

auto WatchlistService::RenameWatchlist(const std::string& watchlist_id,
                                       const std::string& name)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.RenameWatchlist(watchlist_id, name);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.RenameWatchlist(watchlist_id, name);
}

auto WatchlistService::AddWatchlistSymbol(const std::string& watchlist_id,
                                          const std::string& symbol)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.AddWatchlistSymbol(watchlist_id, symbol);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.AddWatchlistSymbol(watchlist_id, symbol);
}

auto WatchlistService::RemoveWatchlistSymbol(const std::string& watchlist_id,
                                             const std::string& symbol)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.RemoveWatchlistSymbol(watchlist_id, symbol);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.RemoveWatchlistSymbol(watchlist_id, symbol);
}

auto WatchlistService::PinWatchlistSymbol(const std::string& watchlist_id,
                                          const std::string& symbol,
                                          bool pinned) -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.PinWatchlistSymbol(watchlist_id, symbol, pinned);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.PinWatchlistSymbol(watchlist_id, symbol, pinned);
}

auto WatchlistService::MoveWatchlistSymbol(const std::string& watchlist_id,
                                           const std::string& symbol,
                                           const std::string& before_symbol)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.MoveWatchlistSymbol(watchlist_id, symbol, before_symbol);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.MoveWatchlistSymbol(watchlist_id, symbol, before_symbol);
}

auto WatchlistService::ArchiveWatchlist(const std::string& watchlist_id,
                                        bool archived) -> WatchlistSummary {
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.ArchiveWatchlist(watchlist_id, archived);
}

auto WatchlistService::DeleteWatchlist(const std::string& watchlist_id)
    -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.DeleteWatchlist(watchlist_id);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.DeleteWatchlist(watchlist_id);
}

auto WatchlistService::MoveSymbolToWatchlist(
    const std::string& source_watchlist_id,
    const std::string& destination_watchlist_id,
    const std::string& symbol) -> WatchlistSummary {
  try {
    providers::tda::WatchlistProvider provider(TdaConfigPath(), kWatchlistsPath);
    return provider.MoveSymbolToWatchlist(source_watchlist_id,
                                          destination_watchlist_id, symbol);
  } catch (const std::exception&) {
  }
  providers::local::WatchlistProvider provider(kWatchlistsPath);
  return provider.MoveSymbolToWatchlist(source_watchlist_id,
                                        destination_watchlist_id, symbol);
}

auto OrderService::PreviewOrder(const OrderIntentRequest& request)
    -> OrderPreviewData {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.PreviewOrder(request);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
    return provider.PreviewOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.PreviewOrder(request);
}

auto OrderService::SubmitOrder(const OrderIntentRequest& request)
    -> OrderSubmissionData {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.SubmitOrder(request);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
    return provider.SubmitOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.SubmitOrder(request);
}

auto OrderService::CancelOrder(const OrderCancelRequest& request)
    -> OrderCancellationData {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.CancelOrder(request);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
    return provider.CancelOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.CancelOrder(request);
}

auto OrderService::ReplaceOrder(const OrderReplaceRequest& request)
    -> OrderReplacementData {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.ReplaceOrder(request);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
    return provider.ReplaceOrder(request);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.ReplaceOrder(request);
}

auto OrderService::GetOpenOrders(const std::string& account_id) const
    -> std::vector<OrderRecordData> {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.GetOpenOrders(account_id);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
    return provider.GetOpenOrders(account_id);
  } catch (const std::exception&) {
  }
  providers::local::OrderProvider provider(kOrdersPath);
  return provider.GetOpenOrders(account_id);
}

auto OrderService::GetOrderHistory(const std::string& account_id) const
    -> std::vector<OrderRecordData> {
  try {
    providers::schwab::OrderProvider provider(SchwabConfigPath(), SchwabTokenPath());
    return provider.GetOrderHistory(account_id);
  } catch (const std::exception&) {
  }
  try {
    providers::tda::OrderProvider provider(TdaConfigPath());
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
  providers::schwab::WorkflowProvider provider(SchwabConfigPath(),
                                               SchwabTokenPath());
  return provider.StartOAuth(request, state, CurrentUtcTimestamp());
}

auto WorkflowService::CompleteSchwabOAuth(
    const SchwabOAuthCompleteRequest& request) -> ConnectionSummary {
  auto& schwab = connection_service_.MutableConnection(Provider::kSchwab);
  providers::schwab::WorkflowProvider provider(SchwabConfigPath(),
                                               SchwabTokenPath());
  schwab = provider.CompleteOAuth(request, schwab, CurrentUtcTimestamp());
  return schwab;
}

auto WorkflowService::CreatePlaidLinkToken(const PlaidLinkTokenRequest& request)
    -> PlaidLinkTokenData {
  auto& plaid = connection_service_.MutableConnection(Provider::kPlaid);
  plaid.status = ConnectionStatus::kConnecting;
  providers::plaid::WorkflowProvider provider(PlaidConfigPath(), PlaidTokenPath());
  return provider.CreateLinkToken(request, CurrentUtcTimestamp(), NextWorkflowId());
}

auto WorkflowService::CompletePlaidLink(
    const PlaidLinkCompleteRequest& request) -> ConnectionSummary {
  auto& plaid = connection_service_.MutableConnection(Provider::kPlaid);
  providers::plaid::WorkflowProvider provider(PlaidConfigPath(), PlaidTokenPath());
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

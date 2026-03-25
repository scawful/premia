#include "provider_service_components.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "premia/infrastructure/secrets/runtime_paths.hpp"
#include "premia/infrastructure/secrets/secret_store.hpp"
#include "Plaid/client.h"
#include "Schwab/client.h"
#include "premia/providers/local/account_detail_provider.hpp"
#include "premia/providers/local/options_provider.hpp"
#include "premia/providers/local/order_provider.hpp"
#include "premia/providers/local/order_template_provider.hpp"
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
constexpr char kOrderTemplatesPath[] = "assets/order_templates.json";

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

auto FormatDecimal(double value) -> std::string {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << value;
  return oss.str();
}

auto ComputeDayChangeFromPositions(const AccountDetail& detail)
    -> AbsolutePercentChange {
  double total_day_pl = 0.0;
  for (const auto& pos : detail.positions) {
    total_day_pl += ParsePriceString(pos.day_profit_loss.amount);
  }
  const double net_liq = ParsePriceString(detail.net_liquidation.amount);
  const double prev = net_liq - total_day_pl;
  const std::string pct =
      prev > 0.001 ? FormatDecimal(total_day_pl / prev * 100.0) : "0.00";
  return AbsolutePercentChange{MakeMoney(FormatDecimal(total_day_pl)), pct};
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

struct PersistedChartAnnotationState {
  std::vector<ChartAnnotation> current;
  std::vector<ChartAnnotationVersionSummary> versions;
  std::map<std::string, std::vector<ChartAnnotation>> version_annotations;
};

auto ChartAnnotationStorageKey(const std::string& account_id,
                               const std::string& symbol) -> std::string {
  return (account_id.empty() ? std::string("default") : account_id) + "|" + symbol;
}

auto SerializeChartAnnotations(const std::vector<ChartAnnotation>& annotations)
    -> pt::ptree {
  pt::ptree items;
  for (const auto& annotation : annotations) {
    pt::ptree item;
    item.put("id", annotation.id);
    item.put("label", annotation.label);
    item.put("price", annotation.price);
    item.put("kind", annotation.kind);
    items.push_back({"", item});
  }
  return items;
}

auto ParseChartAnnotationsTree(const pt::ptree& items)
    -> std::vector<ChartAnnotation> {
  std::vector<ChartAnnotation> annotations;
  for (const auto& item : items) {
    annotations.push_back(ChartAnnotation{item.second.get<std::string>("id", ""),
                                          item.second.get<std::string>("label", ""),
                                          item.second.get<std::string>("price", "0.00"),
                                          item.second.get<std::string>("kind", "annotation")});
  }
  return annotations;
}

auto CurrentChartVersionTimestamp() -> std::string {
  const auto now = std::chrono::system_clock::now();
  const auto now_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_time{};
#if defined(_WIN32)
  gmtime_s(&utc_time, &now_time);
#else
  gmtime_r(&now_time, &utc_time);
#endif
  std::ostringstream stream;
  stream << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
  return stream.str();
}

auto CurrentChartVersionId(const std::string& symbol) -> std::string {
  const auto now = std::chrono::system_clock::now().time_since_epoch();
  const auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  return symbol + ":v:" + std::to_string(millis);
}

auto LoadPersistedChartAnnotationState(const std::string& account_id,
                                       const std::string& symbol)
    -> PersistedChartAnnotationState {
  std::ifstream input(ChartAnnotationStatePath());
  if (!input.good()) {
    return {};
  }

  PersistedChartAnnotationState state;
  try {
    pt::ptree tree;
    pt::read_json(input, tree);
    const auto key = ChartAnnotationStorageKey(account_id, symbol);
    if (auto entry = tree.get_child_optional("annotations." + key)) {
      const bool looks_like_legacy_array =
          !entry->empty() && entry->front().second.find("id") != entry->front().second.not_found();
      if (looks_like_legacy_array) {
        state.current = ParseChartAnnotationsTree(*entry);
      } else {
        if (auto current = entry->get_child_optional("current")) {
          state.current = ParseChartAnnotationsTree(*current);
        }
        if (auto versions = entry->get_child_optional("versions")) {
          for (const auto& version_node : *versions) {
            ChartAnnotationVersionSummary summary;
            summary.id = version_node.second.get<std::string>("id", "");
            summary.saved_at = version_node.second.get<std::string>("savedAt", "");
            summary.annotation_count =
                version_node.second.get<int>("annotationCount", 0);
            state.versions.push_back(summary);
            if (auto annotations = version_node.second.get_child_optional("annotations")) {
              state.version_annotations[summary.id] =
                  ParseChartAnnotationsTree(*annotations);
            }
          }
        }
      }
    }
  } catch (const std::exception&) {
  }
  return state;
}

void SavePersistedChartAnnotationState(const std::string& account_id,
                                       const std::string& symbol,
                                       PersistedChartAnnotationState state) {
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

  const auto key = ChartAnnotationStorageKey(account_id, symbol);
  pt::ptree entry;
  entry.add_child("current", SerializeChartAnnotations(state.current));
  pt::ptree versions_tree;
  for (const auto& version : state.versions) {
    pt::ptree version_tree;
    version_tree.put("id", version.id);
    version_tree.put("savedAt", version.saved_at);
    version_tree.put("annotationCount", version.annotation_count);
    const auto annotations_it = state.version_annotations.find(version.id);
    version_tree.add_child(
        "annotations",
        SerializeChartAnnotations(annotations_it == state.version_annotations.end()
                                      ? std::vector<ChartAnnotation>{}
                                      : annotations_it->second));
    versions_tree.push_back({"", version_tree});
  }
  entry.add_child("versions", versions_tree);
  root.put_child("annotations." + key, entry);

  std::ofstream output(ChartAnnotationStatePath());
  pt::write_json(output, root);
}

auto LoadPersistedChartAnnotations(const std::string& account_id,
                                   const std::string& symbol)
    -> std::vector<ChartAnnotation> {
  return LoadPersistedChartAnnotationState(account_id, symbol).current;
}

auto LoadPersistedChartAnnotationVersions(const std::string& account_id,
                                          const std::string& symbol)
    -> std::vector<ChartAnnotationVersionSummary> {
  return LoadPersistedChartAnnotationState(account_id, symbol).versions;
}

void PushChartAnnotationVersion(PersistedChartAnnotationState& state,
                                const std::string& symbol,
                                const std::vector<ChartAnnotation>& annotations) {
  if (annotations.empty()) {
    return;
  }
  const auto version_id = CurrentChartVersionId(symbol);
  const auto saved_at = CurrentChartVersionTimestamp();
  state.versions.insert(state.versions.begin(),
                        ChartAnnotationVersionSummary{version_id,
                                                      saved_at,
                                                      static_cast<int>(annotations.size())});
  state.version_annotations[version_id] = annotations;
  if (state.versions.size() > 12) {
    const auto removed = state.versions.back().id;
    state.versions.pop_back();
    state.version_annotations.erase(removed);
  }
}

auto BuildChartAnnotations(const std::string& symbol,
                           const std::string& account_id)
    -> std::vector<ChartAnnotation> {
  std::vector<ChartAnnotation> annotations;
  try {
    PortfolioAccountService portfolio_service;
    const auto account = portfolio_service.GetAccountDetailForAccount(account_id);
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

auto BuildChartAnnotationVersions(const std::string& symbol,
                                  const std::string& account_id)
    -> std::vector<ChartAnnotationVersionSummary> {
  try {
    PortfolioAccountService portfolio_service;
    const auto account = portfolio_service.GetAccountDetailForAccount(account_id);
    return LoadPersistedChartAnnotationVersions(account.account_id, symbol);
  } catch (const std::exception&) {
    return {};
  }
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

auto PortfolioAccountService::GetMultiAccountHomeScreen() const
    -> MultiAccountHomeScreen {
  MultiAccountHomeScreen screen;
  std::vector<HoldingRow> all_holdings;
  double aggregate_net_worth = 0.0;
  double aggregate_day_pl = 0.0;

  // Try Schwab — may surface multiple accounts
  try {
    providers::schwab::AccountDetailProvider schwab_provider(SchwabConfigPath(),
                                                             SchwabTokenPath());
    const auto summaries = schwab_provider.ListBrokerageAccounts();
    for (const auto& summary : summaries) {
      try {
        const auto detail =
            schwab_provider.GetAccountDetailForAccount(summary.account_id);
        const auto change = ComputeDayChangeFromPositions(detail);
        MultiAccountSummaryRow row;
        row.provider = Provider::kSchwab;
        row.account_id = detail.account_id;
        row.display_name = summary.display_name;
        row.balance = detail.net_liquidation;
        row.day_change = change;
        row.holdings_count = static_cast<int>(detail.positions.size());
        screen.accounts.push_back(std::move(row));
        const auto holdings =
            MakeHoldingsFromAccount(detail, Provider::kSchwab);
        all_holdings.insert(all_holdings.end(), holdings.begin(),
                            holdings.end());
        aggregate_net_worth +=
            ParsePriceString(detail.net_liquidation.amount);
        aggregate_day_pl += ParsePriceString(change.absolute.amount);
      } catch (const std::exception&) {
      }
    }
  } catch (const std::exception&) {
  }

  // Try IBKR — single account
  try {
    providers::ibkr::AccountDetailProvider ibkr_provider(IbkrConfigPath());
    const auto detail = ibkr_provider.GetAccountDetail();
    const auto change = ComputeDayChangeFromPositions(detail);
    MultiAccountSummaryRow row;
    row.provider = Provider::kIBKR;
    row.account_id = detail.account_id;
    row.display_name = "Interactive Brokers";
    row.balance = detail.net_liquidation;
    row.day_change = change;
    row.holdings_count = static_cast<int>(detail.positions.size());
    screen.accounts.push_back(std::move(row));
    const auto holdings = MakeHoldingsFromAccount(detail, Provider::kIBKR);
    all_holdings.insert(all_holdings.end(), holdings.begin(), holdings.end());
    aggregate_net_worth += ParsePriceString(detail.net_liquidation.amount);
    aggregate_day_pl += ParsePriceString(change.absolute.amount);
  } catch (const std::exception&) {
  }

  // Try TDA — single account
  try {
    providers::tda::AccountDetailProvider tda_provider(TdaConfigPath());
    const auto detail = tda_provider.GetAccountDetail();
    const auto change = ComputeDayChangeFromPositions(detail);
    MultiAccountSummaryRow row;
    row.provider = Provider::kTDA;
    row.account_id = detail.account_id;
    row.display_name = "TDAmeritrade";
    row.balance = detail.net_liquidation;
    row.day_change = change;
    row.holdings_count = static_cast<int>(detail.positions.size());
    screen.accounts.push_back(std::move(row));
    const auto holdings = MakeHoldingsFromAccount(detail, Provider::kTDA);
    all_holdings.insert(all_holdings.end(), holdings.begin(), holdings.end());
    aggregate_net_worth += ParsePriceString(detail.net_liquidation.amount);
    aggregate_day_pl += ParsePriceString(change.absolute.amount);
  } catch (const std::exception&) {
  }

  // Fall back to local when no live provider responded
  if (screen.accounts.empty()) {
    providers::local::AccountDetailProvider local_acct(kAccountPath);
    const auto detail = local_acct.GetAccountDetail();
    const auto change = ComputeDayChangeFromPositions(detail);
    MultiAccountSummaryRow row;
    row.provider = Provider::kInternal;
    row.account_id = detail.account_id;
    row.display_name = "Local Preview";
    row.balance = detail.net_liquidation;
    row.day_change = change;
    row.holdings_count = static_cast<int>(detail.positions.size());
    screen.accounts.push_back(std::move(row));
    providers::local::PortfolioProvider local_portfolio(kPortfolioPath);
    const auto holdings = local_portfolio.GetTopHoldings();
    all_holdings.insert(all_holdings.end(), holdings.begin(), holdings.end());
    aggregate_net_worth += ParsePriceString(detail.net_liquidation.amount);
    aggregate_day_pl += ParsePriceString(change.absolute.amount);
  }

  screen.aggregate_net_worth = MakeMoney(FormatDecimal(aggregate_net_worth));
  const double prev = aggregate_net_worth - aggregate_day_pl;
  const std::string pct =
      prev > 0.001 ? FormatDecimal(aggregate_day_pl / prev * 100.0) : "0.00";
  screen.aggregate_day_change =
      AbsolutePercentChange{MakeMoney(FormatDecimal(aggregate_day_pl)), pct};
  screen.top_holdings = std::move(all_holdings);
  return screen;
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
                                          bool extended_hours,
                                          const std::string& account_id) const
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
      fallback_chart.annotations = BuildChartAnnotations(symbol, account_id);
      fallback_chart.annotation_versions =
          BuildChartAnnotationVersions(symbol, account_id);
      return fallback_chart;
    }
    chart.annotations = BuildChartAnnotations(symbol, account_id);
    chart.annotation_versions = BuildChartAnnotationVersions(symbol, account_id);
    return chart;
  } catch (const std::exception&) {
    auto fallback_chart = fallback;
    fallback_chart.annotations = BuildChartAnnotations(symbol, account_id);
    fallback_chart.annotation_versions =
        BuildChartAnnotationVersions(symbol, account_id);
    return fallback_chart;
  }
}

auto MarketOptionsService::ReplaceChartAnnotations(
    const std::string& symbol,
    const std::vector<ChartAnnotation>& annotations,
    const std::string& account_id) -> ChartScreenData {
  PortfolioAccountService portfolio_service;
  const auto account = portfolio_service.GetAccountDetailForAccount(account_id);

  std::vector<ChartAnnotation> persisted;
  persisted.reserve(annotations.size());
  for (const auto& annotation : annotations) {
    if (annotation.kind == "annotation" || annotation.kind == "entry" ||
        annotation.kind == "stop" || annotation.kind == "target") {
        persisted.push_back(annotation);
    }
  }
  auto state = LoadPersistedChartAnnotationState(account.account_id, symbol);
  PushChartAnnotationVersion(state, symbol, state.current);
  state.current = persisted;
  SavePersistedChartAnnotationState(account.account_id, symbol, state);
  return GetChartScreen(symbol, "1M", "1D", false, account.account_id);
}

auto MarketOptionsService::UpsertChartAnnotation(
    const std::string& symbol, const ChartAnnotation& annotation,
    const std::string& account_id) -> ChartScreenData {
  PortfolioAccountService portfolio_service;
  const auto account = portfolio_service.GetAccountDetailForAccount(account_id);

  auto state = LoadPersistedChartAnnotationState(account.account_id, symbol);
  auto persisted = state.current;
  auto existing = std::find_if(persisted.begin(), persisted.end(),
                               [&annotation](const ChartAnnotation& item) {
                                 return item.id == annotation.id;
                               });
  PushChartAnnotationVersion(state, symbol, state.current);
  if (existing == persisted.end()) {
    persisted.push_back(annotation);
  } else {
    *existing = annotation;
  }
  state.current = persisted;
  SavePersistedChartAnnotationState(account.account_id, symbol, state);
  return GetChartScreen(symbol, "1M", "1D", false, account.account_id);
}

auto MarketOptionsService::DeleteChartAnnotation(
    const std::string& symbol, const std::string& annotation_id,
    const std::string& account_id) -> ChartScreenData {
  PortfolioAccountService portfolio_service;
  const auto account = portfolio_service.GetAccountDetailForAccount(account_id);

  auto state = LoadPersistedChartAnnotationState(account.account_id, symbol);
  auto persisted = state.current;
  PushChartAnnotationVersion(state, symbol, state.current);
  persisted.erase(std::remove_if(persisted.begin(), persisted.end(),
                                 [&annotation_id](const ChartAnnotation& item) {
                                   return item.id == annotation_id;
                                 }),
                  persisted.end());
  state.current = persisted;
  SavePersistedChartAnnotationState(account.account_id, symbol, state);
  return GetChartScreen(symbol, "1M", "1D", false, account.account_id);
}

auto MarketOptionsService::RollbackChartAnnotations(
    const std::string& symbol, const std::string& version_id,
    const std::string& account_id) -> ChartScreenData {
  PortfolioAccountService portfolio_service;
  const auto account = portfolio_service.GetAccountDetailForAccount(account_id);

  auto state = LoadPersistedChartAnnotationState(account.account_id, symbol);
  const auto version_it = state.version_annotations.find(version_id);
  if (version_it == state.version_annotations.end()) {
    throw std::runtime_error("chart annotation version not found");
  }
  PushChartAnnotationVersion(state, symbol, state.current);
  state.current = version_it->second;
  SavePersistedChartAnnotationState(account.account_id, symbol, state);
  return GetChartScreen(symbol, "1M", "1D", false, account.account_id);
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

auto OrderTemplateService::ListOrderTemplates() const
    -> std::vector<OrderTemplate> {
  providers::local::OrderTemplateProvider provider(kOrderTemplatesPath);
  return provider.ListTemplates();
}

auto OrderTemplateService::CreateOrderTemplate(const OrderTemplate& tmpl)
    -> OrderTemplate {
  providers::local::OrderTemplateProvider provider(kOrderTemplatesPath);
  return provider.CreateTemplate(tmpl);
}

auto OrderTemplateService::UpdateOrderTemplate(const std::string& id,
                                               const OrderTemplate& tmpl)
    -> OrderTemplate {
  providers::local::OrderTemplateProvider provider(kOrderTemplatesPath);
  return provider.UpdateTemplate(id, tmpl);
}

auto OrderTemplateService::DeleteOrderTemplate(const std::string& id)
    -> OrderTemplate {
  providers::local::OrderTemplateProvider provider(kOrderTemplatesPath);
  return provider.DeleteTemplate(id);
}

auto OrderTemplateService::PreviewQuickTrade(
    const QuickTradePreviewRequest& request) -> OrderPreviewData {
  // Load the template by scanning the list.
  providers::local::OrderTemplateProvider template_provider(kOrderTemplatesPath);
  const auto templates = template_provider.ListTemplates();
  const OrderTemplate* found = nullptr;
  for (const auto& t : templates) {
    if (t.id == request.template_id) {
      found = &t;
      break;
    }
  }
  if (found == nullptr) {
    throw std::runtime_error("order template not found: " + request.template_id);
  }

  // Resolve the symbol: request symbol takes precedence; fall back to template.
  const auto symbol = !request.symbol.empty() ? request.symbol : found->symbol;
  if (symbol.empty()) {
    throw std::runtime_error("symbol is required for quick-trade preview");
  }

  // Hydrate price from market data (Schwab preferred, local fallback).
  std::string limit_price;
  const auto quote_detail = BuildQuoteFallback(symbol);
  try {
    providers::schwab::MarketDataProvider market_provider(SchwabConfigPath(),
                                                          SchwabTokenPath());
    const auto live = market_provider.GetQuoteDetail(symbol);
    limit_price = live.quote.last_price.amount;
  } catch (const std::exception&) {
    limit_price = quote_detail.quote.last_price.amount;
  }

  // Compute quantity from dollar amount if requested.
  std::string quantity = found->quantity;
  if (found->is_dollar_amount) {
    try {
      const double price = boost::lexical_cast<double>(limit_price);
      const double dollars = boost::lexical_cast<double>(found->quantity);
      if (price > 0.0) {
        const int shares = static_cast<int>(dollars / price);
        quantity = std::to_string(shares);
      }
    } catch (const boost::bad_lexical_cast&) {
      quantity = "0";
    }
  }

  // Build and preview the intent using the local order provider.
  OrderIntentRequest intent;
  intent.account_id = request.account_id;
  intent.symbol = symbol;
  intent.asset_type = found->asset_type.empty() ? "EQUITY" : found->asset_type;
  intent.instruction = found->action;
  intent.quantity = quantity;
  intent.order_type = found->order_type.empty() ? "MARKET" : found->order_type;
  intent.limit_price = (found->order_type == "MARKET") ? "" : limit_price;
  intent.duration = found->time_in_force.empty() ? "DAY" : found->time_in_force;
  intent.session = found->session.empty() ? "NORMAL" : found->session;
  intent.confirm_live = request.confirm_live;

  providers::local::OrderProvider order_provider(kOrdersPath);
  auto preview = order_provider.PreviewOrder(intent);
  // Tag template origin in the first warning slot so callers can identify it.
  preview.warnings.insert(preview.warnings.begin(),
                          "Quick-trade preview from template: " + found->name);
  return preview;
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

// ── RSU vesting helpers ──────────────────────────────────────────────────────

namespace {

constexpr char kRsuGrantsPath[] = "assets/rsu_grants.json";

auto ParseDateTm(const std::string& date_str) -> std::tm {
  std::tm tm{};
  int year = 0, month = 0, day = 0;
  if (std::sscanf(date_str.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_isdst = -1;
    std::mktime(&tm);
  }
  return tm;
}

auto FormatDateTm(const std::tm& tm) -> std::string {
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(4) << (tm.tm_year + 1900) << '-'
      << std::setw(2) << (tm.tm_mon + 1) << '-' << std::setw(2) << tm.tm_mday;
  return oss.str();
}

auto AddMonthsToTm(std::tm base, int months) -> std::tm {
  base.tm_mon += months;
  base.tm_isdst = -1;
  std::mktime(&base);
  return base;
}

}  // namespace

auto ComputeGrantVesting(const GrantConfig& config, std::time_t now)
    -> StockUnitGrant {
  if (now == 0) {
    now = std::time(nullptr);
  }

  const std::tm grant_tm = ParseDateTm(config.grant_date);

  // Standard 4-year/1-year-cliff schedule:
  //   cliff event = total_units / 4
  //   12 quarterly events = remaining units / 12 (last gets remainder)
  const int cliff_units = config.total_units / 4;
  const int post_cliff_units = config.total_units - cliff_units;
  const int per_quarter = post_cliff_units / 12;

  std::vector<VestEvent> schedule;
  schedule.reserve(13);

  // Cliff event
  std::tm cliff_tm = AddMonthsToTm(grant_tm, config.cliff_months);
  const std::time_t cliff_time = std::mktime(&cliff_tm);
  schedule.push_back({FormatDateTm(cliff_tm), cliff_units, now >= cliff_time});

  // 12 quarterly events after cliff
  for (int q = 1; q <= 12; ++q) {
    std::tm vest_tm = AddMonthsToTm(cliff_tm, q * 3);
    const std::time_t vest_time = std::mktime(&vest_tm);
    const int units =
        (q == 12) ? (post_cliff_units - per_quarter * 11) : per_quarter;
    schedule.push_back({FormatDateTm(vest_tm), units, now >= vest_time});
  }

  int vested_units = 0;
  std::string next_vest_date;
  int next_vest_units = 0;
  for (const auto& event : schedule) {
    if (event.vested) {
      vested_units += event.units;
    } else if (next_vest_date.empty()) {
      next_vest_date = event.date;
      next_vest_units = event.units;
    }
  }

  StockUnitGrant grant;
  grant.id = config.id;
  grant.symbol = config.symbol;
  grant.grant_date = config.grant_date;
  grant.total_units = config.total_units;
  grant.vested_units = vested_units;
  grant.unvested_units = config.total_units - vested_units;
  grant.next_vest_date = next_vest_date;
  grant.next_vest_units = next_vest_units;
  grant.vest_progress_percent =
      (config.total_units > 0)
          ? (100.0 * vested_units / config.total_units)
          : 0.0;
  grant.vest_schedule = std::move(schedule);
  return grant;
}

auto RsuOverlayService::GetGrantsWithVesting(std::time_t now) const
    -> std::vector<StockUnitGrant> {
  std::ifstream file(kRsuGrantsPath);
  if (!file.good()) {
    return {};
  }

  pt::ptree tree;
  try {
    pt::read_json(file, tree);
  } catch (const std::exception&) {
    return {};
  }

  std::vector<StockUnitGrant> grants;
  for (const auto& [key, node] : tree.get_child("grants")) {
    GrantConfig config;
    config.id = node.get<std::string>("id", "");
    config.symbol = node.get<std::string>("symbol", "");
    config.grant_date = node.get<std::string>("grant_date", "");
    config.total_units = node.get<int>("total_units", 0);
    config.cliff_months = node.get<int>("cliff_months", 12);
    if (config.id.empty() || config.grant_date.empty() ||
        config.total_units <= 0) {
      continue;
    }
    grants.push_back(ComputeGrantVesting(config, now));
  }
  return grants;
}

}  // namespace premia::core::application::detail

#include "premia/core/application/scaffold_application_service.hpp"

#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "premia/core/application/composition_root.hpp"

#include "provider_service_components.hpp"

namespace premia::core::application {

namespace {

auto FormatUsdAmount(double value) -> std::string {
  std::ostringstream oss;
  oss.precision(2);
  oss << std::fixed << value;
  return oss.str();
}

auto PriceStringToDouble(const std::string& s) -> double {
  if (s.empty()) return 0.0;
  try {
    return std::stod(s);
  } catch (...) {
    return 0.0;
  }
}

}  // namespace

auto ProviderBackedApplicationService::Instance() -> ProviderBackedApplicationService& {
  return CompositionRoot::Instance().AppService();
}

ProviderBackedApplicationService::ProviderBackedApplicationService()
    : connection_service_(std::make_unique<detail::ConnectionService>()),
      portfolio_service_(std::make_unique<detail::PortfolioAccountService>()),
      market_options_service_(std::make_unique<detail::MarketOptionsService>()),
      watchlist_service_(std::make_unique<detail::WatchlistService>()),
      order_service_(std::make_unique<detail::OrderService>()),
      workflow_service_(std::make_unique<detail::WorkflowService>(*connection_service_)),
      rsu_service_(std::make_unique<detail::RsuOverlayService>()) {}

ProviderBackedApplicationService::~ProviderBackedApplicationService() = default;

auto ProviderBackedApplicationService::GetBootstrapData() const -> BootstrapData {
  BootstrapData data;
  data.environment = "development";
  data.feature_flags = {
      {"options", false},
      {"banking", false},
      {"streaming", true},
      {"desktopMigration", true},
  };
  data.connections = connection_service_->GetConnections();
  return data;
}

auto ProviderBackedApplicationService::GetHomeScreenData() const -> HomeScreenData {
  return GetHomeScreenDataForAccount("");
}

auto ProviderBackedApplicationService::GetMultiAccountHomeScreen() const
    -> MultiAccountHomeScreen {
  MultiAccountHomeScreen screen =
      portfolio_service_->GetMultiAccountHomeScreen();
  screen.connections = connection_service_->GetConnections();
  return screen;
}

auto ProviderBackedApplicationService::GetHomeScreenDataForAccount(
    const std::string& account_id) const -> HomeScreenData {
  HomeScreenData data;
  data.connections = connection_service_->GetConnections();
  data.brokerage_accounts = portfolio_service_->ListBrokerageAccounts();
  data.active_account_id = account_id;
  data.portfolio = portfolio_service_->GetPortfolioSummaryForAccount(account_id);
  data.top_holdings = portfolio_service_->GetTopHoldingsForAccount(account_id);
  data.watchlists = watchlist_service_->ListWatchlists();
  data.market = MarketSummary{"open", "2026-03-22T20:00:00Z"};
  if (data.active_account_id.empty()) {
    if (!data.brokerage_accounts.empty()) {
      data.active_account_id = data.brokerage_accounts.front().account_id;
    } else {
      data.active_account_id = GetAccountDetailForAccount("").account_id;
    }
  }
  return data;
}

auto ProviderBackedApplicationService::ListBrokerageAccounts() const
    -> std::vector<BrokerageAccountSummary> {
  return portfolio_service_->ListBrokerageAccounts();
}

auto ProviderBackedApplicationService::GetConnections() const
    -> std::vector<ConnectionSummary> {
  return connection_service_->GetConnections();
}

auto ProviderBackedApplicationService::GetConnection(
    const std::string& provider_key) const -> ConnectionSummary {
  return connection_service_->GetConnection(provider_key);
}

auto ProviderBackedApplicationService::GetPortfolioSummary() const
    -> PortfolioSummary {
  return portfolio_service_->GetPortfolioSummary();
}

auto ProviderBackedApplicationService::GetPortfolioSummaryForAccount(
    const std::string& account_id) const -> PortfolioSummary {
  return portfolio_service_->GetPortfolioSummaryForAccount(account_id);
}

auto ProviderBackedApplicationService::GetTopHoldings() const
    -> std::vector<HoldingRow> {
  return portfolio_service_->GetTopHoldings();
}

auto ProviderBackedApplicationService::GetTopHoldingsForAccount(
    const std::string& account_id) const -> std::vector<HoldingRow> {
  return portfolio_service_->GetTopHoldingsForAccount(account_id);
}

auto ProviderBackedApplicationService::GetAccountDetail() const -> AccountDetail {
  return portfolio_service_->GetAccountDetail();
}

auto ProviderBackedApplicationService::GetAccountDetailForAccount(
    const std::string& account_id) const -> AccountDetail {
  return portfolio_service_->GetAccountDetailForAccount(account_id);
}

auto ProviderBackedApplicationService::GetQuoteDetail(const std::string& symbol) const
    -> QuoteDetail {
  return market_options_service_->GetQuoteDetail(symbol);
}

auto ProviderBackedApplicationService::GetChartScreen(const std::string& symbol,
                                                      const std::string& range,
                                                      const std::string& interval,
                                                      bool extended_hours,
                                                      const std::string& account_id) const
    -> ChartScreenData {
  return market_options_service_->GetChartScreen(symbol, range, interval,
                                                 extended_hours, account_id);
}

auto ProviderBackedApplicationService::ReplaceChartAnnotations(
    const std::string& symbol,
    const std::vector<ChartAnnotation>& annotations,
    const std::string& account_id) -> ChartScreenData {
  return market_options_service_->ReplaceChartAnnotations(symbol, annotations,
                                                          account_id);
}

auto ProviderBackedApplicationService::UpsertChartAnnotation(
    const std::string& symbol, const ChartAnnotation& annotation,
    const std::string& account_id) -> ChartScreenData {
  return market_options_service_->UpsertChartAnnotation(symbol, annotation,
                                                        account_id);
}

auto ProviderBackedApplicationService::DeleteChartAnnotation(
    const std::string& symbol, const std::string& annotation_id,
    const std::string& account_id) -> ChartScreenData {
  return market_options_service_->DeleteChartAnnotation(symbol, annotation_id,
                                                        account_id);
}

auto ProviderBackedApplicationService::RollbackChartAnnotations(
    const std::string& symbol, const std::string& version_id,
    const std::string& account_id) -> ChartScreenData {
  return market_options_service_->RollbackChartAnnotations(symbol, version_id,
                                                           account_id);
}

auto ProviderBackedApplicationService::GetOptionChainSnapshot(
    const std::string& symbol, const std::string& strike_count,
    const std::string& strategy, const std::string& range,
    const std::string& exp_month, const std::string& option_type) const
    -> OptionChainSnapshot {
  return market_options_service_->GetOptionChainSnapshot(symbol, strike_count,
                                                         strategy, range,
                                                         exp_month, option_type);
}

auto ProviderBackedApplicationService::ListWatchlists() const
    -> std::vector<WatchlistSummary> {
  return watchlist_service_->ListWatchlists();
}

auto ProviderBackedApplicationService::GetWatchlistScreen(
    const std::string& watchlist_id) const -> WatchlistScreenData {
  return watchlist_service_->GetWatchlistScreen(watchlist_id);
}

auto ProviderBackedApplicationService::CreateWatchlist(const std::string& name)
    -> WatchlistSummary {
  return watchlist_service_->CreateWatchlist(name);
}

auto ProviderBackedApplicationService::RenameWatchlist(
    const std::string& watchlist_id, const std::string& name) -> WatchlistSummary {
  return watchlist_service_->RenameWatchlist(watchlist_id, name);
}

auto ProviderBackedApplicationService::AddWatchlistSymbol(
    const std::string& watchlist_id, const std::string& symbol)
    -> WatchlistSummary {
  return watchlist_service_->AddWatchlistSymbol(watchlist_id, symbol);
}

auto ProviderBackedApplicationService::RemoveWatchlistSymbol(
    const std::string& watchlist_id, const std::string& symbol)
    -> WatchlistSummary {
  return watchlist_service_->RemoveWatchlistSymbol(watchlist_id, symbol);
}

auto ProviderBackedApplicationService::PinWatchlistSymbol(
    const std::string& watchlist_id, const std::string& symbol, bool pinned)
    -> WatchlistSummary {
  return watchlist_service_->PinWatchlistSymbol(watchlist_id, symbol, pinned);
}

auto ProviderBackedApplicationService::MoveWatchlistSymbol(
    const std::string& watchlist_id, const std::string& symbol,
    const std::string& before_symbol) -> WatchlistSummary {
  return watchlist_service_->MoveWatchlistSymbol(watchlist_id, symbol,
                                                 before_symbol);
}

auto ProviderBackedApplicationService::ArchiveWatchlist(
    const std::string& watchlist_id, bool archived) -> WatchlistSummary {
  return watchlist_service_->ArchiveWatchlist(watchlist_id, archived);
}

auto ProviderBackedApplicationService::DeleteWatchlist(
    const std::string& watchlist_id) -> WatchlistSummary {
  return watchlist_service_->DeleteWatchlist(watchlist_id);
}

auto ProviderBackedApplicationService::MoveSymbolToWatchlist(
    const std::string& source_watchlist_id,
    const std::string& destination_watchlist_id,
    const std::string& symbol) -> WatchlistSummary {
  return watchlist_service_->MoveSymbolToWatchlist(source_watchlist_id,
                                                   destination_watchlist_id,
                                                   symbol);
}

auto ProviderBackedApplicationService::PreviewOrder(
    const OrderIntentRequest& request) -> OrderPreviewData {
  return order_service_->PreviewOrder(request);
}

auto ProviderBackedApplicationService::SubmitOrder(
    const OrderIntentRequest& request) -> OrderSubmissionData {
  return order_service_->SubmitOrder(request);
}

auto ProviderBackedApplicationService::CancelOrder(
    const OrderCancelRequest& request) -> OrderCancellationData {
  return order_service_->CancelOrder(request);
}

auto ProviderBackedApplicationService::ReplaceOrder(
    const OrderReplaceRequest& request) -> OrderReplacementData {
  return order_service_->ReplaceOrder(request);
}

auto ProviderBackedApplicationService::GetOpenOrders(
    const std::string& account_id) const -> std::vector<OrderRecordData> {
  return order_service_->GetOpenOrders(account_id);
}

auto ProviderBackedApplicationService::GetOrderHistory(
    const std::string& account_id) const -> std::vector<OrderRecordData> {
  return order_service_->GetOrderHistory(account_id);
}

auto ProviderBackedApplicationService::GetRSUOverlay() const
    -> RSUOverlayScreen {
  RSUOverlayScreen screen;
  screen.account = portfolio_service_->GetAccountDetail();

  auto grants = rsu_service_->GetGrantsWithVesting();

  // Look up current price for each grant symbol in the account positions.
  for (auto& grant : grants) {
    for (const auto& position : screen.account.positions) {
      if (position.symbol == grant.symbol) {
        grant.current_price = position.average_price;
        break;
      }
    }
    const double price = PriceStringToDouble(grant.current_price.amount);
    grant.vested_value = {FormatUsdAmount(price * grant.vested_units), "USD"};
    grant.unvested_value = {FormatUsdAmount(price * grant.unvested_units), "USD"};
  }

  // Aggregate totals across all grants.
  double total_vested = 0.0;
  double total_unvested = 0.0;
  int total_units_all = 0;
  int total_vested_all = 0;
  for (const auto& grant : grants) {
    total_vested += PriceStringToDouble(grant.vested_value.amount);
    total_unvested += PriceStringToDouble(grant.unvested_value.amount);
    total_units_all += grant.total_units;
    total_vested_all += grant.vested_units;

    if (screen.next_vest_date.empty() && !grant.next_vest_date.empty()) {
      screen.next_vest_date = grant.next_vest_date;
      screen.next_vest_units = grant.next_vest_units;
    }
  }

  screen.total_vested_value = {FormatUsdAmount(total_vested), "USD"};
  screen.total_unvested_value = {FormatUsdAmount(total_unvested), "USD"};
  screen.vest_progress_percent =
      (total_units_all > 0)
          ? (100.0 * total_vested_all / total_units_all)
          : 0.0;
  screen.grants = std::move(grants);
  return screen;
}

auto ProviderBackedApplicationService::CreateLinkToken(
    const PlaidLinkTokenRequest& request) -> PlaidLinkTokenData {
  return workflow_service_->CreateLinkToken(request);
}

auto ProviderBackedApplicationService::StartSchwabOAuth(
    const SchwabOAuthStartRequest& request) -> SchwabOAuthStartData {
  return workflow_service_->StartSchwabOAuth(request);
}

auto ProviderBackedApplicationService::CompleteSchwabOAuth(
    const SchwabOAuthCompleteRequest& request) -> ConnectionSummary {
  return workflow_service_->CompleteSchwabOAuth(request);
}

auto ProviderBackedApplicationService::CreatePlaidLinkToken(
    const PlaidLinkTokenRequest& request) -> PlaidLinkTokenData {
  return workflow_service_->CreatePlaidLinkToken(request);
}

auto ProviderBackedApplicationService::CompletePlaidLink(
    const PlaidLinkCompleteRequest& request) -> ConnectionSummary {
  return workflow_service_->CompletePlaidLink(request);
}

}  // namespace premia::core::application

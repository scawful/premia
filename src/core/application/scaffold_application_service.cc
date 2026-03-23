#include "premia/core/application/scaffold_application_service.hpp"

#include "premia/core/application/composition_root.hpp"

#include "provider_service_components.hpp"

namespace premia::core::application {

auto ProviderBackedApplicationService::Instance() -> ProviderBackedApplicationService& {
  return CompositionRoot::Instance().AppService();
}

ProviderBackedApplicationService::ProviderBackedApplicationService()
    : connection_service_(std::make_unique<detail::ConnectionService>()),
      portfolio_service_(std::make_unique<detail::PortfolioAccountService>()),
      market_options_service_(std::make_unique<detail::MarketOptionsService>()),
      watchlist_service_(std::make_unique<detail::WatchlistService>()),
      order_service_(std::make_unique<detail::OrderService>()),
      workflow_service_(std::make_unique<detail::WorkflowService>(*connection_service_)) {}

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
  HomeScreenData data;
  data.connections = connection_service_->GetConnections();
  data.portfolio = portfolio_service_->GetPortfolioSummary();
  data.top_holdings = portfolio_service_->GetTopHoldings();
  data.watchlists = watchlist_service_->ListWatchlists();
  data.market = MarketSummary{"open", "2026-03-22T20:00:00Z"};
  return data;
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

auto ProviderBackedApplicationService::GetTopHoldings() const
    -> std::vector<HoldingRow> {
  return portfolio_service_->GetTopHoldings();
}

auto ProviderBackedApplicationService::GetAccountDetail() const -> AccountDetail {
  return portfolio_service_->GetAccountDetail();
}

auto ProviderBackedApplicationService::GetQuoteDetail(const std::string& symbol) const
    -> QuoteDetail {
  return market_options_service_->GetQuoteDetail(symbol);
}

auto ProviderBackedApplicationService::GetChartScreen(const std::string& symbol,
                                                      const std::string& range,
                                                      const std::string& interval,
                                                      bool extended_hours) const
    -> ChartScreenData {
  return market_options_service_->GetChartScreen(symbol, range, interval,
                                                 extended_hours);
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

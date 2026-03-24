#ifndef PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP
#define PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP

#include <memory>
#include <string>
#include <vector>

#include "premia/core/application/service_contracts.hpp"

namespace premia::core::application {

class CompositionRoot;

namespace detail {
class ConnectionService;
class PortfolioAccountService;
class MarketOptionsService;
class WatchlistService;
class OrderService;
class WorkflowService;
}  // namespace detail

class ProviderBackedApplicationService : public BrokerConnectionService,
                                         public PortfolioService,
                                         public AccountDetailService,
                                         public MarketDataService,
                                         public OptionsService,
                                         public WatchlistService,
                                         public OrderService,
                                         public BankLinkService,
                                         public ConnectionWorkflowService {
 public:
  static auto Instance() -> ProviderBackedApplicationService&;

  ~ProviderBackedApplicationService();

  auto GetBootstrapData() const -> BootstrapData;
  auto GetHomeScreenData() const -> HomeScreenData;
  auto GetHomeScreenDataForAccount(const std::string& account_id) const
      -> HomeScreenData;
  auto ListBrokerageAccounts() const -> std::vector<BrokerageAccountSummary>;
  auto GetPortfolioSummaryForAccount(const std::string& account_id) const
      -> PortfolioSummary;
  auto GetTopHoldingsForAccount(const std::string& account_id) const
      -> std::vector<HoldingRow>;
  auto GetAccountDetailForAccount(const std::string& account_id) const
      -> AccountDetail;

  auto GetConnections() const -> std::vector<ConnectionSummary> override;
  auto GetConnection(const std::string& provider_key) const
      -> ConnectionSummary override;

  auto GetPortfolioSummary() const -> PortfolioSummary override;
  auto GetTopHoldings() const -> std::vector<HoldingRow> override;
  auto GetAccountDetail() const -> AccountDetail override;

  auto GetQuoteDetail(const std::string& symbol) const -> QuoteDetail override;
  auto GetChartScreen(const std::string& symbol, const std::string& range,
                      const std::string& interval,
                      bool extended_hours) const -> ChartScreenData override;
  auto GetOptionChainSnapshot(const std::string& symbol,
                              const std::string& strike_count,
                              const std::string& strategy,
                              const std::string& range,
                              const std::string& exp_month,
                              const std::string& option_type) const
      -> OptionChainSnapshot override;

  auto ListWatchlists() const -> std::vector<WatchlistSummary> override;
  auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> WatchlistScreenData override;
  auto CreateWatchlist(const std::string& name) -> WatchlistSummary override;
  auto RenameWatchlist(const std::string& watchlist_id,
                       const std::string& name) -> WatchlistSummary override;
  auto AddWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol) -> WatchlistSummary override;
  auto RemoveWatchlistSymbol(const std::string& watchlist_id,
                             const std::string& symbol) -> WatchlistSummary override;
  auto PinWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol,
                          bool pinned) -> WatchlistSummary override;
  auto MoveWatchlistSymbol(const std::string& watchlist_id,
                           const std::string& symbol,
                           const std::string& before_symbol) -> WatchlistSummary override;

  auto PreviewOrder(const OrderIntentRequest& request)
      -> OrderPreviewData override;
  auto SubmitOrder(const OrderIntentRequest& request)
      -> OrderSubmissionData override;
  auto CancelOrder(const OrderCancelRequest& request)
      -> OrderCancellationData override;
  auto ReplaceOrder(const OrderReplaceRequest& request)
      -> OrderReplacementData override;
  auto GetOpenOrders(const std::string& account_id) const
      -> std::vector<OrderRecordData> override;
  auto GetOrderHistory(const std::string& account_id) const
      -> std::vector<OrderRecordData> override;

  auto CreateLinkToken(const PlaidLinkTokenRequest& request)
      -> PlaidLinkTokenData override;

  auto StartSchwabOAuth(const SchwabOAuthStartRequest& request)
      -> SchwabOAuthStartData override;
  auto CompleteSchwabOAuth(const SchwabOAuthCompleteRequest& request)
      -> ConnectionSummary override;
  auto CreatePlaidLinkToken(const PlaidLinkTokenRequest& request)
      -> PlaidLinkTokenData override;
  auto CompletePlaidLink(const PlaidLinkCompleteRequest& request)
      -> ConnectionSummary override;

 private:
  friend class CompositionRoot;

  ProviderBackedApplicationService();

  std::unique_ptr<detail::ConnectionService> connection_service_;
  std::unique_ptr<detail::PortfolioAccountService> portfolio_service_;
  std::unique_ptr<detail::MarketOptionsService> market_options_service_;
  std::unique_ptr<detail::WatchlistService> watchlist_service_;
  std::unique_ptr<detail::OrderService> order_service_;
  std::unique_ptr<detail::WorkflowService> workflow_service_;
};

using ScaffoldApplicationService = ProviderBackedApplicationService;

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP

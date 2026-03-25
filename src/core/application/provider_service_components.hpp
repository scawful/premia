#ifndef PREMIA_CORE_APPLICATION_PROVIDER_SERVICE_COMPONENTS_HPP
#define PREMIA_CORE_APPLICATION_PROVIDER_SERVICE_COMPONENTS_HPP

#include <string>
#include <vector>

#include "premia/core/application/service_contracts.hpp"

namespace premia::core::application::detail {

auto MakeHoldingsFromAccount(const AccountDetail& detail,
                             premia::core::domain::Provider provider)
    -> std::vector<HoldingRow>;

class ConnectionService {
 public:
  ConnectionService();

  auto GetConnections() const -> std::vector<ConnectionSummary>;
  auto GetConnection(const std::string& provider_key) const -> ConnectionSummary;
  auto MutableConnection(core::domain::Provider provider) -> ConnectionSummary&;

 private:
  std::vector<ConnectionSummary> connections_;
};

class PortfolioAccountService {
 public:
  auto ListBrokerageAccounts() const -> std::vector<BrokerageAccountSummary>;
  auto GetPortfolioSummary() const -> PortfolioSummary;
  auto GetPortfolioSummaryForAccount(const std::string& account_id) const
      -> PortfolioSummary;
  auto GetTopHoldings() const -> std::vector<HoldingRow>;
  auto GetTopHoldingsForAccount(const std::string& account_id) const
      -> std::vector<HoldingRow>;
  auto GetAccountDetail() const -> AccountDetail;
  auto GetAccountDetailForAccount(const std::string& account_id) const
      -> AccountDetail;
};

class MarketOptionsService {
 public:
  auto GetQuoteDetail(const std::string& symbol) const -> QuoteDetail;
  auto GetChartScreen(const std::string& symbol, const std::string& range,
                      const std::string& interval,
                      bool extended_hours,
                      const std::string& account_id = "") const
      -> ChartScreenData;
  auto ReplaceChartAnnotations(const std::string& symbol,
                               const std::vector<ChartAnnotation>& annotations,
                               const std::string& account_id = "")
      -> ChartScreenData;
  auto UpsertChartAnnotation(const std::string& symbol,
                             const ChartAnnotation& annotation,
                             const std::string& account_id = "")
      -> ChartScreenData;
  auto DeleteChartAnnotation(const std::string& symbol,
                             const std::string& annotation_id,
                             const std::string& account_id = "")
      -> ChartScreenData;
  auto GetOptionChainSnapshot(const std::string& symbol,
                              const std::string& strike_count,
                              const std::string& strategy,
                              const std::string& range,
                              const std::string& exp_month,
                              const std::string& option_type) const
      -> OptionChainSnapshot;
};

class WatchlistService {
 public:
  auto ListWatchlists() const -> std::vector<WatchlistSummary>;
  auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> WatchlistScreenData;
  auto CreateWatchlist(const std::string& name) -> WatchlistSummary;
  auto RenameWatchlist(const std::string& watchlist_id,
                       const std::string& name) -> WatchlistSummary;
  auto AddWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol) -> WatchlistSummary;
  auto RemoveWatchlistSymbol(const std::string& watchlist_id,
                             const std::string& symbol) -> WatchlistSummary;
  auto PinWatchlistSymbol(const std::string& watchlist_id,
                          const std::string& symbol,
                          bool pinned) -> WatchlistSummary;
  auto MoveWatchlistSymbol(const std::string& watchlist_id,
                           const std::string& symbol,
                           const std::string& before_symbol) -> WatchlistSummary;
  auto ArchiveWatchlist(const std::string& watchlist_id, bool archived)
      -> WatchlistSummary;
  auto DeleteWatchlist(const std::string& watchlist_id) -> WatchlistSummary;
  auto MoveSymbolToWatchlist(const std::string& source_watchlist_id,
                             const std::string& destination_watchlist_id,
                             const std::string& symbol) -> WatchlistSummary;
};

class OrderService {
 public:
  auto PreviewOrder(const OrderIntentRequest& request) -> OrderPreviewData;
  auto SubmitOrder(const OrderIntentRequest& request) -> OrderSubmissionData;
  auto CancelOrder(const OrderCancelRequest& request) -> OrderCancellationData;
  auto ReplaceOrder(const OrderReplaceRequest& request) -> OrderReplacementData;
  auto GetOpenOrders(const std::string& account_id) const
      -> std::vector<OrderRecordData>;
  auto GetOrderHistory(const std::string& account_id) const
      -> std::vector<OrderRecordData>;
};

class WorkflowService {
 public:
  explicit WorkflowService(ConnectionService& connection_service);

  auto CreateLinkToken(const PlaidLinkTokenRequest& request) -> PlaidLinkTokenData;
  auto StartSchwabOAuth(const SchwabOAuthStartRequest& request)
      -> SchwabOAuthStartData;
  auto CompleteSchwabOAuth(const SchwabOAuthCompleteRequest& request)
      -> ConnectionSummary;
  auto CreatePlaidLinkToken(const PlaidLinkTokenRequest& request)
      -> PlaidLinkTokenData;
  auto CompletePlaidLink(const PlaidLinkCompleteRequest& request)
      -> ConnectionSummary;

 private:
  auto NextWorkflowId() -> unsigned long long;
  auto CurrentUtcTimestamp() const -> std::string;

  ConnectionService& connection_service_;
  unsigned long long workflow_counter_ = 0;
};

}  // namespace premia::core::application::detail

#endif  // PREMIA_CORE_APPLICATION_PROVIDER_SERVICE_COMPONENTS_HPP

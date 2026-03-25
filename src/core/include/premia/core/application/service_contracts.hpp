#ifndef PREMIA_CORE_APPLICATION_SERVICE_CONTRACTS_HPP
#define PREMIA_CORE_APPLICATION_SERVICE_CONTRACTS_HPP

#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"

namespace premia::core::application {

class BrokerConnectionService {
 public:
  virtual ~BrokerConnectionService() = default;

  virtual auto GetConnections() const -> std::vector<ConnectionSummary> = 0;
  virtual auto GetConnection(const std::string& provider_key) const
      -> ConnectionSummary = 0;
};

class PortfolioService {
 public:
  virtual ~PortfolioService() = default;

  virtual auto GetPortfolioSummary() const -> PortfolioSummary = 0;
  virtual auto GetTopHoldings() const -> std::vector<HoldingRow> = 0;
};

class AccountDetailService {
 public:
  virtual ~AccountDetailService() = default;

  virtual auto GetAccountDetail() const -> AccountDetail = 0;
};

class MarketDataService {
 public:
  virtual ~MarketDataService() = default;

  virtual auto GetQuoteDetail(const std::string& symbol) const -> QuoteDetail = 0;
  virtual auto GetChartScreen(const std::string& symbol, const std::string& range,
                              const std::string& interval,
                              bool extended_hours,
                              const std::string& account_id = "") const
      -> ChartScreenData = 0;
  virtual auto ReplaceChartAnnotations(
      const std::string& symbol,
      const std::vector<ChartAnnotation>& annotations,
      const std::string& account_id = "") -> ChartScreenData = 0;
  virtual auto UpsertChartAnnotation(const std::string& symbol,
                                     const ChartAnnotation& annotation,
                                     const std::string& account_id = "")
      -> ChartScreenData = 0;
  virtual auto DeleteChartAnnotation(const std::string& symbol,
                                     const std::string& annotation_id,
                                     const std::string& account_id = "")
      -> ChartScreenData = 0;
  virtual auto RollbackChartAnnotations(const std::string& symbol,
                                        const std::string& version_id,
                                        const std::string& account_id = "")
      -> ChartScreenData = 0;
};

class OptionsService {
 public:
  virtual ~OptionsService() = default;

  virtual auto GetOptionChainSnapshot(const std::string& symbol,
                                      const std::string& strike_count,
                                      const std::string& strategy,
                                      const std::string& range,
                                      const std::string& exp_month,
                                      const std::string& option_type) const
      -> OptionChainSnapshot = 0;
};

class WatchlistService {
 public:
  virtual ~WatchlistService() = default;

  virtual auto ListWatchlists() const -> std::vector<WatchlistSummary> = 0;
  virtual auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> WatchlistScreenData = 0;
  virtual auto CreateWatchlist(const std::string& name) -> WatchlistSummary = 0;
  virtual auto RenameWatchlist(const std::string& watchlist_id,
                               const std::string& name) -> WatchlistSummary = 0;
  virtual auto AddWatchlistSymbol(const std::string& watchlist_id,
                                  const std::string& symbol) -> WatchlistSummary = 0;
  virtual auto RemoveWatchlistSymbol(const std::string& watchlist_id,
                                     const std::string& symbol) -> WatchlistSummary = 0;
  virtual auto PinWatchlistSymbol(const std::string& watchlist_id,
                                  const std::string& symbol,
                                  bool pinned) -> WatchlistSummary = 0;
  virtual auto MoveWatchlistSymbol(const std::string& watchlist_id,
                                   const std::string& symbol,
                                   const std::string& before_symbol)
      -> WatchlistSummary = 0;
  virtual auto ArchiveWatchlist(const std::string& watchlist_id, bool archived)
      -> WatchlistSummary = 0;
  virtual auto DeleteWatchlist(const std::string& watchlist_id)
      -> WatchlistSummary = 0;
  virtual auto MoveSymbolToWatchlist(const std::string& source_watchlist_id,
                                     const std::string& destination_watchlist_id,
                                     const std::string& symbol)
      -> WatchlistSummary = 0;
};

class BankLinkService {
 public:
  virtual ~BankLinkService() = default;

  virtual auto CreateLinkToken(const PlaidLinkTokenRequest& request)
      -> PlaidLinkTokenData = 0;
};

class ConnectionWorkflowService {
 public:
  virtual ~ConnectionWorkflowService() = default;

  virtual auto StartSchwabOAuth(const SchwabOAuthStartRequest& request)
      -> SchwabOAuthStartData = 0;
  virtual auto CompleteSchwabOAuth(const SchwabOAuthCompleteRequest& request)
      -> ConnectionSummary = 0;
  virtual auto CreatePlaidLinkToken(const PlaidLinkTokenRequest& request)
      -> PlaidLinkTokenData = 0;
  virtual auto CompletePlaidLink(const PlaidLinkCompleteRequest& request)
      -> ConnectionSummary = 0;
};

class OrderService {
 public:
  virtual ~OrderService() = default;

  virtual auto PreviewOrder(const OrderIntentRequest& request)
      -> OrderPreviewData = 0;
  virtual auto SubmitOrder(const OrderIntentRequest& request)
      -> OrderSubmissionData = 0;
  virtual auto CancelOrder(const OrderCancelRequest& request)
      -> OrderCancellationData = 0;
  virtual auto ReplaceOrder(const OrderReplaceRequest& request)
      -> OrderReplacementData = 0;
  virtual auto GetOpenOrders(const std::string& account_id) const
      -> std::vector<OrderRecordData> = 0;
  virtual auto GetOrderHistory(const std::string& account_id) const
      -> std::vector<OrderRecordData> = 0;
};

class RSUOverlayService {
 public:
  virtual ~RSUOverlayService() = default;

  virtual auto GetRSUOverlay() const -> RSUOverlayScreen = 0;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SERVICE_CONTRACTS_HPP

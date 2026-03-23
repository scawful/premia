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
                              bool extended_hours) const -> ChartScreenData = 0;
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
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SERVICE_CONTRACTS_HPP

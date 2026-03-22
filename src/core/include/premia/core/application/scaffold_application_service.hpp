#ifndef PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP
#define PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP

#include <string>
#include <vector>

#include "premia/core/application/service_contracts.hpp"

namespace premia::core::application {

namespace domain = premia::core::domain;

class CompositionRoot;

class ScaffoldApplicationService : public BrokerConnectionService,
                                   public PortfolioService,
                                   public AccountDetailService,
                                   public MarketDataService,
                                   public OptionsService,
                                   public WatchlistService,
                                   public BankLinkService,
                                   public ConnectionWorkflowService {
 public:
  static auto Instance() -> ScaffoldApplicationService&;

  auto GetBootstrapData() const -> BootstrapData;
  auto GetHomeScreenData() const -> HomeScreenData;

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

  ScaffoldApplicationService();

  auto FindConnection(domain::Provider provider) -> ConnectionSummary&;
  auto FindConnection(domain::Provider provider) const -> const ConnectionSummary&;
  auto BuildQuoteDetailForSymbol(const std::string& symbol) const -> QuoteDetail;
  auto BuildChartSeriesForSymbol(const std::string& symbol,
                                 bool extended_hours) const -> ChartSeries;
  auto NextWorkflowId() -> unsigned long long;
  auto CurrentUtcTimestamp() const -> std::string;

  std::vector<ConnectionSummary> connections_;
  std::vector<WatchlistSummary> watchlists_;
  std::vector<HoldingRow> holdings_;
  mutable unsigned long long workflow_counter_ = 0;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_SCAFFOLD_APPLICATION_SERVICE_HPP

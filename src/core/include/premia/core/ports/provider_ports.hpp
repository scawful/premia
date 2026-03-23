#ifndef PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP
#define PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP

#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"

namespace premia::core::ports {

using premia::core::application::ChartScreenData;
using premia::core::application::ConnectionSummary;
using premia::core::application::HoldingRow;
using premia::core::application::AccountDetail;
using premia::core::application::OptionChainSnapshot;
using premia::core::application::PortfolioSummary;
using premia::core::application::QuoteDetail;
using premia::core::application::WatchlistScreenData;
using premia::core::application::WatchlistSummary;

class ConnectionProviderPort {
 public:
  virtual ~ConnectionProviderPort() = default;

  virtual auto GetConnectionSummary() const -> ConnectionSummary = 0;
};

class PortfolioProviderPort {
 public:
  virtual ~PortfolioProviderPort() = default;

  virtual auto GetPortfolioSummary() const -> PortfolioSummary = 0;
  virtual auto GetTopHoldings() const -> std::vector<HoldingRow> = 0;
};

class AccountDetailProviderPort {
 public:
  virtual ~AccountDetailProviderPort() = default;

  virtual auto GetAccountDetail() const -> AccountDetail = 0;
};

class MarketDataProviderPort {
 public:
  virtual ~MarketDataProviderPort() = default;

  virtual auto GetQuoteDetail(const std::string& symbol) const -> QuoteDetail = 0;
  virtual auto GetChartScreen(const std::string& symbol, const std::string& range,
                              const std::string& interval,
                              bool extended_hours) const -> ChartScreenData = 0;
};

class OptionsProviderPort {
 public:
  virtual ~OptionsProviderPort() = default;

  virtual auto GetOptionChainSnapshot(const std::string& symbol,
                                      const std::string& strike_count,
                                      const std::string& strategy,
                                      const std::string& range,
                                      const std::string& exp_month,
                                      const std::string& option_type) const
      -> OptionChainSnapshot = 0;
};

class WatchlistProviderPort {
 public:
  virtual ~WatchlistProviderPort() = default;

  virtual auto ListWatchlists() const -> std::vector<WatchlistSummary> = 0;
  virtual auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> WatchlistScreenData = 0;
};

class WatchlistMutationProviderPort {
 public:
  virtual ~WatchlistMutationProviderPort() = default;

  virtual auto CreateWatchlist(const std::string& name) -> WatchlistSummary = 0;
  virtual auto RenameWatchlist(const std::string& watchlist_id,
                               const std::string& name) -> WatchlistSummary = 0;
  virtual auto AddWatchlistSymbol(const std::string& watchlist_id,
                                  const std::string& symbol) -> WatchlistSummary = 0;
  virtual auto RemoveWatchlistSymbol(const std::string& watchlist_id,
                                     const std::string& symbol) -> WatchlistSummary = 0;
};

class OrderProviderPort {
 public:
  virtual ~OrderProviderPort() = default;

  virtual auto PreviewOrder(
      const premia::core::application::OrderIntentRequest& request)
      -> premia::core::application::OrderPreviewData = 0;
  virtual auto SubmitOrder(
      const premia::core::application::OrderIntentRequest& request)
      -> premia::core::application::OrderSubmissionData = 0;
  virtual auto CancelOrder(
      const premia::core::application::OrderCancelRequest& request)
      -> premia::core::application::OrderCancellationData = 0;
  virtual auto ReplaceOrder(
      const premia::core::application::OrderReplaceRequest& request)
      -> premia::core::application::OrderReplacementData = 0;
};

}  // namespace premia::core::ports

#endif  // PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP

#ifndef PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP
#define PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP

#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"

namespace premia::core::ports {

using premia::core::application::ChartScreenData;
using premia::core::application::ConnectionSummary;
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
};

class MarketDataProviderPort {
 public:
  virtual ~MarketDataProviderPort() = default;

  virtual auto GetQuoteDetail(const std::string& symbol) const -> QuoteDetail = 0;
  virtual auto GetChartScreen(const std::string& symbol, const std::string& range,
                              const std::string& interval,
                              bool extended_hours) const -> ChartScreenData = 0;
};

class WatchlistProviderPort {
 public:
  virtual ~WatchlistProviderPort() = default;

  virtual auto ListWatchlists() const -> std::vector<WatchlistSummary> = 0;
  virtual auto GetWatchlistScreen(const std::string& watchlist_id) const
      -> WatchlistScreenData = 0;
};

}  // namespace premia::core::ports

#endif  // PREMIA_CORE_PORTS_PROVIDER_PORTS_HPP

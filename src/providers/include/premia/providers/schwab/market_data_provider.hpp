#ifndef PREMIA_PROVIDERS_SCHWAB_MARKET_DATA_PROVIDER_HPP
#define PREMIA_PROVIDERS_SCHWAB_MARKET_DATA_PROVIDER_HPP

#include <string>

#include "Schwab/client.h"
#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::schwab {

class MarketDataProvider : public core::ports::MarketDataProviderPort {
 public:
  MarketDataProvider(std::string config_path, std::string token_path);

  auto GetQuoteDetail(const std::string& symbol) const
      -> core::application::QuoteDetail override;
  auto GetChartScreen(const std::string& symbol, const std::string& range,
                      const std::string& interval,
                      bool extended_hours) const
      -> core::application::ChartScreenData override;

 private:
  auto LoadAuthorizedClient(::premia::schwab::Client& client) const -> bool;

  std::string config_path_;
  std::string token_path_;
};

}  // namespace premia::providers::schwab

#endif  // PREMIA_PROVIDERS_SCHWAB_MARKET_DATA_PROVIDER_HPP

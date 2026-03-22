#ifndef PREMIA_PROVIDERS_LOCAL_PORTFOLIO_PROVIDER_HPP
#define PREMIA_PROVIDERS_LOCAL_PORTFOLIO_PROVIDER_HPP

#include <string>
#include <vector>

#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::local {

class PortfolioProvider : public core::ports::PortfolioProviderPort {
 public:
  explicit PortfolioProvider(std::string path);

  auto GetPortfolioSummary() const -> core::application::PortfolioSummary override;
  auto GetTopHoldings() const -> std::vector<core::application::HoldingRow> override;

 private:
  auto LoadPortfolioSummary() const -> core::application::PortfolioSummary;
  auto LoadHoldings() const -> std::vector<core::application::HoldingRow>;

  std::string path_;
};

}  // namespace premia::providers::local

#endif  // PREMIA_PROVIDERS_LOCAL_PORTFOLIO_PROVIDER_HPP

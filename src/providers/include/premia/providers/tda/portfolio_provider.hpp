#ifndef PREMIA_PROVIDERS_TDA_PORTFOLIO_PROVIDER_HPP
#define PREMIA_PROVIDERS_TDA_PORTFOLIO_PROVIDER_HPP

#include <string>
#include <vector>

#include "TDAmeritrade/data/Account.hpp"
#include "premia/core/ports/provider_ports.hpp"

namespace premia::providers::tda {

class PortfolioProvider : public core::ports::PortfolioProviderPort {
 public:
  explicit PortfolioProvider(std::string config_path);

  auto GetPortfolioSummary() const -> core::application::PortfolioSummary override;
  auto GetTopHoldings() const -> std::vector<core::application::HoldingRow> override;

 private:
  auto LoadAccount() const -> ::premia::tda::Account;
  auto HasUsableConfig() const -> bool;

  std::string config_path_;
};

}  // namespace premia::providers::tda

#endif  // PREMIA_PROVIDERS_TDA_PORTFOLIO_PROVIDER_HPP

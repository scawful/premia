#ifndef PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP
#define PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP

#include "premia/core/application/scaffold_application_service.hpp"

namespace premia::core::application {

class CompositionRoot {
 public:
  static auto Instance() -> CompositionRoot&;

  auto AppService() -> ProviderBackedApplicationService&;
  auto BrokerConnections() -> BrokerConnectionService&;
  auto Portfolio() -> PortfolioService&;
  auto AccountDetails() -> AccountDetailService&;
  auto MarketData() -> MarketDataService&;
  auto Options() -> OptionsService&;
  auto Watchlists() -> WatchlistService&;
  auto Orders() -> OrderService&;
  auto Workflows() -> ConnectionWorkflowService&;

 private:
  CompositionRoot() = default;

  ProviderBackedApplicationService app_service_;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP

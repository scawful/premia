#ifndef PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP
#define PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP

#include "premia/core/application/scaffold_application_service.hpp"

namespace premia::core::application {

class CompositionRoot {
 public:
  static auto Instance() -> CompositionRoot&;

  auto AppService() -> ScaffoldApplicationService&;
  auto BrokerConnections() -> BrokerConnectionService&;
  auto Portfolio() -> PortfolioService&;
  auto MarketData() -> MarketDataService&;
  auto Watchlists() -> WatchlistService&;
  auto Workflows() -> ConnectionWorkflowService&;

 private:
  CompositionRoot() = default;

  ScaffoldApplicationService app_service_;
};

}  // namespace premia::core::application

#endif  // PREMIA_CORE_APPLICATION_COMPOSITION_ROOT_HPP

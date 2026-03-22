#include "premia/core/application/composition_root.hpp"

namespace premia::core::application {

auto CompositionRoot::Instance() -> CompositionRoot& {
  static CompositionRoot instance;
  return instance;
}

auto CompositionRoot::AppService() -> ScaffoldApplicationService& {
  return app_service_;
}

auto CompositionRoot::BrokerConnections() -> BrokerConnectionService& {
  return app_service_;
}

auto CompositionRoot::Portfolio() -> PortfolioService& {
  return app_service_;
}

auto CompositionRoot::AccountDetails() -> AccountDetailService& {
  return app_service_;
}

auto CompositionRoot::MarketData() -> MarketDataService& {
  return app_service_;
}

auto CompositionRoot::Options() -> OptionsService& {
  return app_service_;
}

auto CompositionRoot::Watchlists() -> WatchlistService& {
  return app_service_;
}

auto CompositionRoot::Workflows() -> ConnectionWorkflowService& {
  return app_service_;
}

}  // namespace premia::core::application

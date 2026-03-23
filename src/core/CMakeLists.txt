add_library(
  premia_core STATIC
  application/composition_root.cc
  application/provider_service_components.cc
  application/scaffold_application_service.cc
)

target_include_directories(
  premia_core
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_core PUBLIC cxx_std_17)

target_link_libraries(
  premia_core
  PUBLIC
  premia_infrastructure
  premia_provider_local_account_detail
  premia_provider_local_options
  premia_provider_local_orders
  premia_provider_local_portfolio
  premia_provider_local_watchlists
  premia_provider_tda_account_detail
  premia_provider_tda_orders
  premia_provider_tda_options
  premia_provider_tda_portfolio
  premia_provider_tda_watchlists
  premia_provider_schwab_account_detail
  premia_provider_schwab_market_data
  premia_provider_schwab_options
  premia_provider_schwab_workflows
  premia_provider_plaid_workflows
  schwab
  plaid
)

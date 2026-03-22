add_library(
  premia_provider_local_watchlists
  local/watchlist_provider.cc
)

target_include_directories(
  premia_provider_local_watchlists
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_local_watchlists PUBLIC cxx_std_17)

add_library(
  premia_provider_local_portfolio
  local/portfolio_provider.cc
)

target_include_directories(
  premia_provider_local_portfolio
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_local_portfolio PUBLIC cxx_std_17)

add_library(
  premia_provider_tda_watchlists
  tda/watchlist_provider.cc
)

target_include_directories(
  premia_provider_tda_watchlists
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_tda_watchlists PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_tda_watchlists
  PRIVATE
  tdameritrade
)

add_library(
  premia_provider_tda_portfolio
  tda/portfolio_provider.cc
)

target_include_directories(
  premia_provider_tda_portfolio
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_tda_portfolio PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_tda_portfolio
  PRIVATE
  tdameritrade
)

add_library(
  premia_provider_schwab_market_data
  schwab/market_data_provider.cc
)

target_include_directories(
  premia_provider_schwab_market_data
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_schwab_market_data PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_schwab_market_data
  PRIVATE
  schwab
)

add_library(
  premia_provider_schwab_workflows
  schwab/workflow_provider.cc
)

target_include_directories(
  premia_provider_schwab_workflows
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_schwab_workflows PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_schwab_workflows
  PRIVATE
  schwab
)

add_library(
  premia_provider_plaid_workflows
  plaid/workflow_provider.cc
)

target_include_directories(
  premia_provider_plaid_workflows
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_plaid_workflows PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_plaid_workflows
  PRIVATE
  plaid
)

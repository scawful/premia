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
  premia_provider_local_account_detail
  local/account_detail_provider.cc
)

target_include_directories(
  premia_provider_local_account_detail
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_local_account_detail PUBLIC cxx_std_17)

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
  premia_provider_tda_account_detail
  tda/account_detail_provider.cc
)

target_include_directories(
  premia_provider_tda_account_detail
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_tda_account_detail PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_tda_account_detail
  PRIVATE
  tdameritrade
)

add_library(
  premia_provider_schwab_account_detail
  schwab/account_detail_provider.cc
)

target_include_directories(
  premia_provider_schwab_account_detail
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_schwab_account_detail PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_schwab_account_detail
  PRIVATE
  schwab
)

add_library(
  premia_provider_local_options
  local/options_provider.cc
)

target_include_directories(
  premia_provider_local_options
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_local_options PUBLIC cxx_std_17)

add_library(
  premia_provider_local_orders
  local/order_provider.cc
)

target_include_directories(
  premia_provider_local_orders
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_local_orders PUBLIC cxx_std_17)

add_library(
  premia_provider_tda_options
  tda/options_provider.cc
)

target_include_directories(
  premia_provider_tda_options
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_tda_options PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_tda_options
  PRIVATE
  tdameritrade
)

add_library(
  premia_provider_schwab_options
  schwab/options_provider.cc
)

target_include_directories(
  premia_provider_schwab_options
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_schwab_options PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_schwab_options
  PRIVATE
  schwab
)

add_library(
  premia_provider_tda_orders
  tda/order_provider.cc
)

target_include_directories(
  premia_provider_tda_orders
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_tda_orders PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_tda_orders
  PRIVATE
  tdameritrade
)

add_library(
  premia_provider_schwab_orders
  schwab/order_provider.cc
)

target_include_directories(
  premia_provider_schwab_orders
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_provider_schwab_orders PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_schwab_orders
  PRIVATE
  schwab
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
  premia_provider_ibkr_account_detail
  ibkr/account_detail_provider.cc
)

target_include_directories(
  premia_provider_ibkr_account_detail
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/service/InteractiveBrokers/tws
)

target_compile_features(premia_provider_ibkr_account_detail PUBLIC cxx_std_17)

target_link_libraries(
  premia_provider_ibkr_account_detail
  PRIVATE
  TWS
  ${BOOST_LIBRARIES}
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

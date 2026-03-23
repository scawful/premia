# PremiaTest using GoogleTest -------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  premia_test
  PremiaTest.cpp
  service/tdameritrade_test.cc
  service/schwab_test.cc
  service/plaid_test.cc
  ../src/service/TDAmeritrade/parser.cc
  ../src/service/TDAmeritrade/socket.cc
  ../src/service/TDAmeritrade/client.cc
  ../src/service/TDAmeritrade/data/Quote.cpp
  ../src/service/TDAmeritrade/data/OptionChain.cpp
  ../src/service/TDAmeritrade/data/Account.cpp
  ../src/service/TDAmeritrade/data/Watchlist.cpp
  ../src/service/TDAmeritrade/data/Order.cpp
  ../src/service/TDAmeritrade/data/UserPrincipals.cpp
  ../src/service/TDAmeritrade/data/PriceHistory.cpp
  ../src/service/CoinbasePro/Account.cpp
  ../src/service/CoinbasePro/Client.cpp
  ../src/service/CoinbasePro/Product.cpp
  ../src/service/Schwab/client.cc
  ../src/service/Plaid/client.cc
  ../src/service/Plaid/parser.cc
  ../src/service/Plaid/data/Account.cpp
  ../src/service/Plaid/data/Transaction.cpp
  ../src/service/Plaid/data/Institution.cpp
  ../src/service/Plaid/data/Balance.cpp
)

add_executable(
  premia_provider_core_test
  providers/ibkr_guardrail_test.cc
  providers/local_order_provider_test.cc
  providers/schwab_guardrail_test.cc
  providers/tda_guardrail_test.cc
  core/order_service_test.cc
)

add_executable(
  premia_api_integration_test
  api/api_integration_test.cc
)

target_include_directories(premia_test
  PUBLIC
  ../src
  ../src/app
  ../src/lib
  ../src/service
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(premia_provider_core_test
  PUBLIC
  ../src
  ../src/app
  ../src/core/include
  ../src/providers/include
  ../src/core/application
  ../src/service
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(premia_api_integration_test
  PUBLIC
  ../src
  ../src/core/include
  ../src/providers/include
  ../src/service
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
  premia_test
  curl
  ${BOOST_LIBRARIES}
  OpenSSL::SSL
  OpenSSL::Crypto
  absl::strings
  absl::str_format
  gmock
  gtest_main
  gtest
)

target_link_libraries(
  premia_provider_core_test
  premia_core
  tdameritrade
  schwab
  plaid
  curl
  ${BOOST_LIBRARIES}
  OpenSSL::SSL
  OpenSSL::Crypto
  absl::strings
  absl::str_format
  gmock
  gtest_main
  gtest
)

target_link_libraries(
  premia_api_integration_test
  curl
  ${BOOST_LIBRARIES}
  OpenSSL::SSL
  OpenSSL::Crypto
  gtest_main
  gtest
)

target_compile_definitions(
  premia_api_integration_test
  PRIVATE
  PREMIA_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  PREMIA_API_BIN="$<TARGET_FILE:premia_api>"
)

add_dependencies(premia_api_integration_test premia_api)

if(WIN32)
  target_link_libraries(premia_test ws2_32)
  target_link_libraries(premia_provider_core_test ws2_32)
  target_link_libraries(premia_api_integration_test ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(premia_test)
gtest_discover_tests(premia_provider_core_test)
gtest_discover_tests(premia_api_integration_test)

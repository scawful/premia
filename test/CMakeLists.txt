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
)

target_include_directories(premia_test
  PUBLIC
  ../src
  ../src/app
  ../src/lib
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

if(WIN32)
  target_link_libraries(premia_test ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(premia_test)

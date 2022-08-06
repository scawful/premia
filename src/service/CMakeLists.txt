add_subdirectory(TDAmeritrade)

set(
  PREMIA_SERVICE_TDA_SRC
  TDAmeritrade/Parser.cpp
  TDAmeritrade/Socket.cpp
  TDAmeritrade/Client.cpp
  TDAmeritrade/Data/Quote.cpp 
  TDAmeritrade/Data/OptionChain.cpp 
  TDAmeritrade/Data/Account.cpp
  TDAmeritrade/Data/Watchlist.cpp
  TDAmeritrade/Data/Order.cpp
  TDAmeritrade/Data/UserPrincipals.cpp
  TDAmeritrade/Data/PriceHistory.cpp 
)

set(
  PREMIA_SERVICE_CB_SRC
  CoinbasePro/Account.cpp 
  CoinbasePro/Client.cpp 
  CoinbasePro/Product.cpp
)

set(
  PREMIA_SERVICE_IBKR_SRC
  InteractiveBrokers/Client.cpp
  InteractiveBrokers/Data/ContractSamples.cpp
  InteractiveBrokers/Data/OrderSamples.cpp
  InteractiveBrokers/Data/ScannerSubscriptionSamples.cpp
  InteractiveBrokers/Data/AvailableAlgoParams.cpp
  InteractiveBrokers/Data/AccountSummaryTags.cpp
  InteractiveBrokers/Utils.cpp
)

include_directories(../app)

add_library(tdameritrade STATIC ${PREMIA_SERVICE_TDA_SRC})
target_link_libraries(
  tdameritrade 
  curl 
  OpenSSL::SSL
  ${BOOST_LIBRARIES} 
)


add_library(coinbase STATIC ${PREMIA_SERVICE_CB_SRC})
target_link_libraries(
  coinbase 
  curl 
  OpenSSL::SSL
  ${BOOST_LIBRARIES} 
)

set(TWS_PATH "InteractiveBrokers/tws")
file(GLOB TWS_SOURCES ${TWS_PATH}/*.cpp)
add_library("TWS" STATIC ${TWS_SOURCES})
target_include_directories("TWS" PUBLIC ${TWS_PATH})

add_library(interactive_brokers STATIC ${PREMIA_SERVICE_IBKR_SRC})
include_directories(
  InteractiveBrokers
)
target_link_libraries(
  interactive_brokers 
  curl 
  OpenSSL::SSL
  TWS
  ${BOOST_LIBRARIES} 
)

add_library(halext STATIC Halext/Client.cpp Halext/User.cpp)
target_link_libraries(
  halext 
  curl 
  OpenSSL::SSL
  ${BOOST_LIBRARIES} 
)

if (WIN32) 
  target_link_libraries(tdameritrade ws2_32)
  target_link_libraries(coinbase ws2_32)
  target_link_libraries(interactive_brokers ws2_32)
  target_link_libraries(halext ws2_32)
endif()


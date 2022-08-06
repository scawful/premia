set(
  PREMIA_SERVICE_IBKR_SRC
  Client.cpp
  Data/ContractSamples.cpp
  Data/OrderSamples.cpp
  Data/ScannerSubscriptionSamples.cpp
  Data/AvailableAlgoParams.cpp
  Data/AccountSummaryTags.cpp
  Utils.cpp
)


set(TWS_PATH "tws")
file(GLOB TWS_SOURCES ${TWS_PATH}/*.cpp)
add_library("TWS" STATIC ${TWS_SOURCES})
target_include_directories("TWS" PUBLIC ${TWS_PATH})

add_library(interactive_brokers STATIC ${PREMIA_SERVICE_IBKR_SRC})
include_directories(
  .
)
target_link_libraries(
  interactive_brokers 
  curl 
  OpenSSL::SSL
  TWS
  ${BOOST_LIBRARIES} 
)

if (WIN32) 
  target_link_libraries(interactive_brokers ws2_32)
endif()
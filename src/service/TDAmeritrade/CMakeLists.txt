set(
  PREMIA_SERVICE_TDA_SRC
  client.cc
  parser.cc
  socket.cc
  data/Quote.cpp
  data/OptionChain.cpp
  data/Account.cpp
  data/Watchlist.cpp
  data/Order.cpp
  data/UserPrincipals.cpp
  data/PriceHistory.cpp
)

add_library(tdameritrade ${PREMIA_SERVICE_TDA_SRC})
target_include_directories(tdameritrade
  PUBLIC
  ./
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(tdameritrade
  PUBLIC
    absl::strings
    absl::str_format
  PRIVATE
    ${BOOST_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    curl
)

if(WIN32)
  target_link_libraries(tdameritrade PRIVATE ws2_32)
endif()

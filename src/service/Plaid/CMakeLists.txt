set(PREMIA_SERVICE_PLAID_SRC
  client.cc
  parser.cc
  data/Account.cpp
  data/Transaction.cpp
  data/Institution.cpp
  data/Balance.cpp
)

add_library(plaid ${PREMIA_SERVICE_PLAID_SRC})

target_include_directories(plaid
  PUBLIC
    ./
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(plaid
  PRIVATE
    premia_infrastructure
    ${BOOST_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    curl
)

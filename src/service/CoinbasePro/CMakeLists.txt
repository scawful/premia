set(
  PREMIA_SERVICE_CB_SRC
  Account.cpp 
  Client.cpp 
  Product.cpp
)

add_library(coinbase STATIC ${PREMIA_SERVICE_CB_SRC})
target_link_libraries(
  coinbase 
  curl 
  OpenSSL
  ${BOOST_LIBRARIES} 
)

if (WIN32) 
  target_link_libraries(coinbase ws2_32)
endif()
set(PREMIA_SERVICE_SCHWAB_SRC
  client.cc
)

add_library(schwab ${PREMIA_SERVICE_SCHWAB_SRC})

target_include_directories(schwab
  PUBLIC
    ./
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(schwab
  PRIVATE
    ${BOOST_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    curl
)

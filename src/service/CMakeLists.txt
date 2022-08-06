add_subdirectory(TDAmeritrade)
add_subdirectory(CoinbasePro)
add_subdirectory(InteractiveBrokers)
include_directories(../app)

add_library(halext STATIC Halext/Client.cpp Halext/User.cpp)
target_link_libraries(
  halext 
  curl 
  OpenSSL::SSL
  ${BOOST_LIBRARIES} 
)

if (WIN32) 
  target_link_libraries(halext ws2_32)
endif()


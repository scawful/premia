# PremiaTest using GoogleTest ------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  premia_test
  PremiaTest.cpp
  service/tdameritrade_test.cc
  ../src/service/TDAmeritrade/handler/tdameritrade_service.cc
  ../src/service/TDAmeritrade/parser.cc
  ../src/service/TDAmeritrade/socket.cc
  ../src/service/TDAmeritrade/client.cc
  ../src/service/TDAmeritrade/Data/Quote.cpp 
  ../src/service/TDAmeritrade/Data/OptionChain.cpp 
  ../src/service/TDAmeritrade/Data/Account.cpp
  ../src/service/TDAmeritrade/Data/Watchlist.cpp
  ../src/service/TDAmeritrade/Data/Order.cpp
  ../src/service/TDAmeritrade/Data/UserPrincipals.cpp
  ../src/service/TDAmeritrade/Data/PriceHistory.cpp 
  ../src/service/IEXCloud/Client.cpp 
  ../src/service/IEXCloud/Parser.cpp
  ../src/service/CoinbasePro/Account.cpp 
  ../src/service/CoinbasePro/Client.cpp 
  ../src/service/CoinbasePro/Product.cpp
)

include_directories(
  premia_test PUBLIC 
  ../src
  ../src/lib  
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
)

target_link_libraries(
  premia_test
  curl
  ${BOOST_LIBRARIES} 
  ${SDL2_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  gtest_main
  gtest
  protobuf::libprotobuf
  gRPC::grpc++_reflection
  gRPC::grpc
  gRPC::grpc++
  tdameritrade
  tda-service
)

if (WIN32) 
    target_link_libraries(premia_test ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(premia_test)
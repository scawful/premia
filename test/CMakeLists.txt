# PremiaTest using GoogleTest ------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  premia_test
  PremiaTest.cpp
  ServiceTestSuite/TDA.cpp
  ServiceTestSuite/CBP.cpp
  ServiceTestSuite/IEX.cpp
  ../src/service/Halext/Client.cpp 
  ../src/service/Halext/User.cpp    
  ../src/service/IEXCloud/Client.cpp 
  ../src/service/IEXCloud/Parser.cpp
  ../src/service/CoinbasePro/Account.cpp 
  ../src/service/CoinbasePro/Client.cpp 
  ../src/service/CoinbasePro/Product.cpp
  ../src/service/TDAmeritrade/Parser.cpp
  ../src/service/TDAmeritrade/Socket.cpp
  ../src/service/TDAmeritrade/Client.cpp
  ../src/service/TDAmeritrade/Data/Quote.cpp 
  ../src/service/TDAmeritrade/Data/OptionChain.cpp 
  ../src/service/TDAmeritrade/Data/Account.cpp
  ../src/service/TDAmeritrade/Data/Watchlist.cpp
  ../src/service/TDAmeritrade/Data/Order.cpp
  ../src/service/TDAmeritrade/Data/UserPrincipals.cpp
  ../src/service/TDAmeritrade/Data/PriceHistory.cpp 
)

include_directories(
  premia_test PUBLIC 
  ../src
  ../src/lib  
  ${PROTOBUF_INCLUDE_DIRS}
)

target_link_libraries(
  premia_test
  ${BOOST_LIBRARIES} 
  ${SDL2_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  curl 
  OpenSSL::SSL 
  gtest_main
  gtest
)

if (WIN32) 
    target_link_libraries(premia_test ws2_32)
endif()

include(GoogleTest)
gtest_discover_tests(premia_test)
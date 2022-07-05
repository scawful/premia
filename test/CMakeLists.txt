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
  ../src/Services/Halext/Client.cpp 
  ../src/Services/Halext/User.cpp    
  ../src/Services/IEXCloud/Client.cpp 
  ../src/Services/IEXCloud/Parser.cpp
  ../src/Services/CoinbasePro/Account.cpp 
  ../src/Services/CoinbasePro/Client.cpp 
  ../src/Services/CoinbasePro/Product.cpp
  ../src/Services/TDAmeritrade/Parser.cpp
  ../src/Services/TDAmeritrade/Socket.cpp
  ../src/Services/TDAmeritrade/Client.cpp
  ../src/Services/TDAmeritrade/Data/Quote.cpp 
  ../src/Services/TDAmeritrade/Data/OptionChain.cpp 
  ../src/Services/TDAmeritrade/Data/Account.cpp
  ../src/Services/TDAmeritrade/Data/Watchlist.cpp
  ../src/Services/TDAmeritrade/Data/Order.cpp
  ../src/Services/TDAmeritrade/Data/UserPrincipals.cpp
  ../src/Services/TDAmeritrade/Data/PriceHistory.cpp 
)

include_directories(
  premia_test PUBLIC 
  libs/fred/include
  ${PROTOBUF_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}/../src/Protocols
)

target_link_libraries(
  premia_test
  ${BOOST_LIBRARIES} 
  ${SDL2_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  premia_proto
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
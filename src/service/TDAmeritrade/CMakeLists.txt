set(PROTO_FILES
    proto/account.proto
    proto/option_chain.proto
)

add_library(tda_proto ${PROTO_FILES})
target_link_libraries(tda_proto
  PUBLIC
  gRPC::grpc++_reflection
  gRPC::grpc
  gRPC::grpc++
  protobuf::libprotobuf
)
target_include_directories(tda_proto 
  PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR}/service/TDAmeritrade
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET tda_proto LANGUAGE cpp)
protobuf_generate(
  TARGET tda_proto 
  LANGUAGE grpc 
  GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc 
  PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)

set(
  PREMIA_SERVICE_TDA_SRC
  Parser.cpp
  Socket.cpp
  Client.cpp
  data/Quote.cpp 
  data/OptionChain.cpp 
  data/Account.cpp
  data/Watchlist.cpp
  data/Order.cpp
  data/UserPrincipals.cpp
  data/PriceHistory.cpp 
  handler/account_handler.cc
  handler/option_chain_handler.cc
)

add_library(tdameritrade ${PREMIA_SERVICE_TDA_SRC})
target_include_directories(tdameritrade 
  PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}/service/TDAmeritrade
)
target_link_libraries(tdameritrade
  PRIVATE
    ${BOOST_LIBRARIES}
    OpenSSL::SSL
    curl 
    gRPC::grpc++_reflection
    gRPC::grpc
    gRPC::grpc++
    protobuf::libprotobuf
    tda_proto
)

if (WIN32) 
  target_link_libraries(tdameritrade PRIVATE ws2_32)
endif()
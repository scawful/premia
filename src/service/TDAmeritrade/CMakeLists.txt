# find_package(gRPC REQUIRED)

# set(PROTO_FILES
#     proto/account.proto
#     proto/option_chain.proto
# )

# # protobuf source files go into the lib just like any other CPP source file
# add_library(tda-service ${PROTO_FILES})
# target_link_libraries(tda-service
#     PUBLIC
#         protobuf::libprotobuf
#         gRPC::grpc
#         gRPC::grpc++
# )

# target_include_directories(tda-service
#     PUBLIC
#         ${CMAKE_CURRENT_BINARY_DIR}
# )

# get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

# # compile the message types
# protobuf_generate(TARGET tda-service LANGUAGE cpp)

# # compile the GRPC services
# protobuf_generate(
#   TARGET
#     tda-service
#   LANGUAGE
#     grpc
#   GENERATE_EXTENSIONS
#     .grpc.pb.h
#     .grpc.pb.cc
#   PLUGIN
#     "protoc-gen-grpc=${grpc_cpp_plugin_location}"
# )

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
  # handler/account_handler.cc
  # handler/option_chain_handler.cc
)

add_library(tdameritrade ${PREMIA_SERVICE_TDA_SRC})
target_include_directories(tdameritrade 
  PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROTOBUF_INCLUDE_PATH}
)

target_link_libraries(tdameritrade
  PRIVATE
    ${BOOST_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    curl 
    # gRPC::grpc++_reflection
    # gRPC::grpc
    # gRPC::grpc++
    # protobuf::libprotobuf
    # tda-service
)

if (WIN32) 
  target_link_libraries(tdameritrade PRIVATE ws2_32)
endif()

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    account.proto
    option_chain.proto
)

#
# Add Library target with protobuf sources
#
add_library(tda_proto ${PROTO_FILES})
target_link_libraries(tda_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(
  tda_proto PUBLIC 
  ${CMAKE_CURRENT_BINARY_DIR}
)

#
# Compile protobuf and grpc files in tda_proto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET tda_proto LANGUAGE cpp)
protobuf_generate(TARGET tda_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
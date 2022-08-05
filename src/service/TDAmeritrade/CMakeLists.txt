# Protobuf library
include(FindProtobuf)

find_package(Protobuf REQUIRED)

include_directories(
  ${PROTOBUF_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

PROTOBUF_GENERATE_CPP(
  PROTO_SRCS 
  PROTO_HDRS 
  protos/account.proto
  protos/option_chain.proto
)

set_source_files_properties(
  ${PROTO_SRCS} 
  ${PROTO_HDRS} 
  PROPERTIES GENERATED TRUE
)

add_library(premia_proto ${PROTO_HDRS} ${PROTO_SRCS})
target_link_libraries(premia_proto INTERFACE ${PROTO_LIBRARIES})
SET_SOURCE_FILES_PROPERTIES(${PROTO_SRC} ${PROTO_INCL} PROPERTIES GENERATED TRUE)
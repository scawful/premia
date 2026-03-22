add_executable(
  premia_api
  src/main.cc
  src/http/json.cc
  src/http/server.cc
)

target_include_directories(
  premia_api
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
  premia_api
  premia_core
  Threads::Threads
)

if(WIN32)
  target_link_libraries(premia_api ws2_32)
endif()

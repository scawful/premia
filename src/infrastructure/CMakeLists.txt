add_library(
  premia_infrastructure STATIC
  secrets/secret_store.cc
  secrets/runtime_paths.cc
)

target_include_directories(
  premia_infrastructure
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_infrastructure PUBLIC cxx_std_17)

if(APPLE)
  find_library(SECURITY_FRAMEWORK Security REQUIRED)
  find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
  target_link_libraries(
    premia_infrastructure
    PUBLIC
    ${SECURITY_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
  )
endif()

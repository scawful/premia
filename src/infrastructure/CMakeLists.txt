add_library(
  premia_infrastructure STATIC
  secrets/runtime_paths.cc
)

target_include_directories(
  premia_infrastructure
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_infrastructure PUBLIC cxx_std_17)

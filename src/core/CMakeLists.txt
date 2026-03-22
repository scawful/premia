add_library(
  premia_core STATIC
  application/scaffold_application_service.cc
)

target_include_directories(
  premia_core
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(premia_core PUBLIC cxx_std_17)

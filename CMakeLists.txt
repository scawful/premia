cmake_minimum_required(VERSION 3.15)
project(premia VERSION 2.0.0)

# C++ Standard ----------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDL_DISABLE_IMMINTRIN_H")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_DISABLE_IMMINTRIN_H")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS OFF)

# Homebrew prefix (Apple Silicon uses /opt/homebrew) --------------------------
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()

# OpenSSL ---------------------------------------------------------------------
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
endif()

# Boost (ARM homebrew preferred over /usr/local Intel install) ----------------
cmake_policy(SET CMP0167 NEW)
set(BOOST_ROOT "/opt/homebrew")
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# SDL2 ------------------------------------------------------------------------
find_package(SDL2 REQUIRED)

# Threads / OpenGL ------------------------------------------------------------
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Abseil (system install via brew) --------------------------------------------
find_package(absl CONFIG REQUIRED)
set(
  ABSL_TARGETS
  absl::strings
  absl::flags
  absl::status
  absl::statusor
  absl::examine_stack
  absl::stacktrace
  absl::base
  absl::config
  absl::core_headers
  absl::raw_logging_internal
  absl::failure_signal_handler
  absl::flat_hash_map
)

# Version header --------------------------------------------------------------
set(PREMIA_VERSION_MAJOR ${premia_VERSION_MAJOR})
set(PREMIA_VERSION_MINOR ${premia_VERSION_MINOR})
set(PREMIA_VERSION_PATCH ${premia_VERSION_PATCH})
configure_file(
    inc/premia_config.h.in
    ${CMAKE_BINARY_DIR}/inc/premia_config.h
)

include_directories(include)
include_directories(inc)
include_directories(${CMAKE_BINARY_DIR}/inc)
add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(test)

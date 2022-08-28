# CMake Specifications --------------------------------------------------------
cmake_minimum_required(VERSION 3.6)

# C++ Standard Specifications -------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_LINKER_FLAGS \"-Wl,--no-undefined -Wl,--no-undefined\")
set(BUILD_SHARED_LIBS ON)

# OpenSSL library -------------------------------------------------------------
if(APPLE)
    set(OPENSSL_INCLUDE_DIR 
      "/usr/local/Cellar/openssl@1.1/1.1.1q/include")
    set(OPENSSL_CRYPTO_LIBRARY 
      "/usr/local/Cellar/openssl@1.1/1.1.1q/lib/libcrypto.dylib")
    set(OPENSSL_SSL_LIBRARY 
      "/usr/local/Cellar/openssl@1.1/1.1.1q/lib/libssl.dylib")
endif()
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    if(APPLE)
        include_directories(/usr/local/Cellar/openssl@1.1/1.1.1q/include)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1q/lib/libssl.dylib)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1q/lib/libcrypto.dylib)
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
    endif()
endif()

# Premia by Justin Scofield ---------------------------------------------------
project(premia VERSION 0.40)
include_directories(include)

# Boost library ---------------------------------------------------------------
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
find_package(SDL2 REQUIRED)

# Abseil Standard Specifications ----------------------------------------------
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_CXX_STANDARD 17)
set(ABSL_USE_GOOGLETEST_HEAD ON)
set(ABSL_ENABLE_INSTALL ON)
add_subdirectory(src/lib/abseil-cpp)
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

add_subdirectory(src/lib/protobuf)
set(Protobuf_BUILD_TESTS OFF)
set(PROTOBUF_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")
include_directories(${PROTOBUF_INCLUDE_PATH})

set(PROTOBUF_LIBRARY "/usr/local/Cellar/protobuf/21.5/lib/libprotobuf.dylib") # The protobuf library
set(PROTOBUF_PROTOC_LIBRARY "/usr/local/Cellar/protobuf/21.5/lib/libprotoc.dylib")   #- The protoc library
set(PROTOBUF_INCLUDE_DIR "/usr/local/Cellar/protobuf/21.5/include") #- The include directory for protocol buffers
set(PROTOBUF_PROTOC_EXECUTABLE "/usr/local/Cellar/protobuf/21.5/bin") #- The protoc compiler
find_package(gRPC REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
# CMake Specifications --------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

# C++ Standard Specifications -------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MODULE_LINKER_FLAGS \"-Wl,--no-undefined -Wl,--no-undefined\")
set(BUILD_SHARED_LIBS ON)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Premia by Justin Scofield ---------------------------------------------------------------------------------
project(premia VERSION 0.40)
include_directories(include)

# Boost library ---------------------------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# Abseil Standard Specifications ----------------------------------------------
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_CXX_STANDARD 17)
set(ABSL_USE_GOOGLETEST_HEAD ON)
set(ABSL_ENABLE_INSTALL ON)
add_subdirectory(src/lib/abseil-cpp)

# OpenSSL library -------------------------------------------------------------------------------------------
if(APPLE)
    set(OPENSSL_INCLUDE_DIR "/usr/local/Cellar/openssl@1.1/1.1.1o/include")
    set(OPENSSL_CRYPTO_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1o/lib/libcrypto.dylib")
    set(OPENSSL_SSL_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1o/lib/libssl.dylib")
endif()
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    if(APPLE)
        include_directories(/usr/local/Cellar/openssl@1.1/1.1.1o/include)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1o/lib/libssl.dylib)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1o/lib/libcrypto.dylib)
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
    endif()
endif()

add_subdirectory(src/lib/SDL)

set(PROTOBUF_INCLUDE_PATH ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")

include_directories(${PROTOBUF_INCLUDE_PATH})

# Premia program --------------------------------------------------------------------------------------------
add_subdirectory(src)

# Premia tests ----------------------------------------------------------------------------------------------
add_subdirectory(test)
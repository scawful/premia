# CMake Specifications --------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

# C++ Standard Specifications -------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Premia by Justin Scofield ---------------------------------------------------------------------------------
project(Premia VERSION 0.40)
include_directories(include)

# Boost library ---------------------------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})


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
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    endif()
endif()

add_subdirectory(src/lib/SDL)

# Premia program --------------------------------------------------------------------------------------------
add_subdirectory(src)

# Premia tests ----------------------------------------------------------------------------------------------
add_subdirectory(test)
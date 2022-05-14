# CMake Specifications -------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

# C++ Standard Specifications ------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# Premia by Justin Scofield --------------------------------------------------------------------------------------
project(Premia VERSION 0.40)
include_directories(include)

# Boost library --------------------------------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS ON)
set(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM "iostreams thread system filesystem") #required by gnuplot-iostream
separate_arguments(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM)
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# SDL libraries --------------------------------------------------------------------------------------------------
find_package(SDL2 REQUIRED)

# cURL library ---------------------------------------------------------------------------------------------------
find_package(CURL REQUIRED)

# OpenSSL library ----------------------------------------------------x--------------------------------------------
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

# ImGui library --------------------------------------------------------------------------------------------------
set(IMGUI_PATH  "libs/imgui")                            # Set where the ImGui files are stored
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)             # Compile as static library 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

# ImPlot library -------------------------------------------------------------------------------------------------
set(IMPLOT_PATH "libs/implot")
file(GLOB IMPLOT_SOURCES ${IMPLOT_PATH}/*.cpp) 
add_library("ImPlot" STATIC ${IMPLOT_SOURCES})
target_include_directories("ImPlot" PUBLIC ${IMGUI_PATH})

# Premia Executable Linkage --------------------------------------------------------------------------------------
add_executable(
    Premia 
    src/Premia.cpp 
    # Model
    src/Model/Model.cpp
    src/Model/Chart/ChartModel.cpp
    src/Model/Options/OptionsModel.cpp
    src/Model/Core/WatchlistModel.cpp
    # View + View Manager 
    src/View/Workspace.cpp
    src/View/ViewManager.cpp
    src/View/Core/PrimaryView.cpp
    src/View/Core/ConsoleView.cpp
    src/View/Menu/MenuView.cpp
    src/View/Login/LoginView.cpp
    src/View/Options/OptionChain.cpp
    src/View/Account/AccountView.cpp
    src/View/Watchlist/WatchlistView.cpp
    src/View/Chart/ChartView.cpp
    src/View/Chart/Subview/CandleChart.cpp
    src/View/Chart/Subview/LinePlotChart.cpp
    # Controller 
    src/Controller/Controller.cpp    
    # Services 
    src/Services/Halext/Client.cpp 
    src/Services/Halext/User.cpp    
    src/Services/IEXCloud/Client.cpp 
    src/Services/IEXCloud/Parser.cpp
    src/Services/CoinbasePro/Account.cpp 
    src/Services/CoinbasePro/Client.cpp 
    src/Services/CoinbasePro/Product.cpp
    src/Services/TDAmeritrade/Parser.cpp
    src/Services/TDAmeritrade/Socket.cpp
    src/Services/TDAmeritrade/Client.cpp
    src/Services/TDAmeritrade/Data/Quote.cpp 
    src/Services/TDAmeritrade/Data/OptionChain.cpp 
    src/Services/TDAmeritrade/Data/Account.cpp
    src/Services/TDAmeritrade/Data/Watchlist.cpp
    src/Services/TDAmeritrade/Data/Order.cpp
    src/Services/TDAmeritrade/Data/UserPrincipals.cpp
    src/Services/TDAmeritrade/Data/PriceHistory.cpp 
    # GUI libraries 
    ${IMGUI_PATH}/imgui.cpp 
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp 
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp 
    ${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
    ${IMPLOT_PATH}/implot.cpp 
    ${IMPLOT_PATH}/implot_items.cpp 
    ${IMPLOT_PATH}/implot_demo.cpp
)
include_directories(Premia "assets")
include_directories(Premia "src/Services/")
include_directories(Premia "src/")
target_link_libraries(
    Premia 
    ${BOOST_LIBRARIES} 
    ${SDL2_LIBRARIES} 
    ${OPENGL_LIBRARIES} 
    ImGui 
    ImPlot
    curl
    OpenSSL::SSL 
)

if (WIN32) 
    target_link_libraries(Premia ws2_32)
endif()

# PremiaTest using GoogleTest ------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  PremiaTest
  tests/PremiaTest.cpp
  tests/ServiceTestSuite/TDA.cpp
  tests/ServiceTestSuite/CBP.cpp
  tests/ServiceTestSuite/IEX.cpp
# tests/ServiceTestSuite/FRED.cpp
  src/Services/Halext/Client.cpp 
  src/Services/Halext/User.cpp    
  src/Services/IEXCloud/Client.cpp 
  src/Services/IEXCloud/Parser.cpp
  src/Services/CoinbasePro/Account.cpp 
  src/Services/CoinbasePro/Client.cpp 
  src/Services/CoinbasePro/Product.cpp
  src/Services/TDAmeritrade/Parser.cpp
  src/Services/TDAmeritrade/Socket.cpp
  src/Services/TDAmeritrade/Client.cpp
  src/Services/TDAmeritrade/Data/Quote.cpp 
  src/Services/TDAmeritrade/Data/OptionChain.cpp 
  src/Services/TDAmeritrade/Data/Account.cpp
  src/Services/TDAmeritrade/Data/Watchlist.cpp
  src/Services/TDAmeritrade/Data/Order.cpp
  src/Services/TDAmeritrade/Data/UserPrincipals.cpp
  src/Services/TDAmeritrade/Data/PriceHistory.cpp 
)

include_directories(PremiaTest PUBLIC libs/fred/include)
find_library(FREDCPP_LIBRARY fredcpp HINTS libs/fred)

target_link_libraries(
  PremiaTest
  ${BOOST_LIBRARIES} 
  ${SDL2_LIBRARIES} 
  ${OPENGL_LIBRARIES}
# ${FREDCPP_LIBRARY}
  curl 
  OpenSSL::SSL 
  gtest_main
  gtest
)

if (WIN32) 
    target_link_libraries(PremiaTest ws2_32)
endif()


include(GoogleTest)
gtest_discover_tests(PremiaTest)

# Assets Folder Link ---------------------------------------------------------------------------------------------
set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
                   DEPENDS ${destination}
                   COMMENT "symbolic link resources folder from ${source} => ${destination}")

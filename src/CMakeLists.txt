# cmake version
cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-O3")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

# set the project name
project(Premia VERSION 1.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

# include-ception
include_directories(include)

# Boost library --------------------------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS ON)

# look for package
set(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM "iostreams thread system filesystem") #required by gnuplot-iostream
separate_arguments(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM)

# find the boost libraries
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM})
# find_package(Boost_thread REQUIRED)
# find_package(Boost_system REQUIRED)
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )


# SDL libraries --------------------------------------------------------------------------------------------
find_package(SDL2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)


# cURL library --------------------------------------------------------------------------------------------
find_package(CURL REQUIRED)

# MySQL library --------------------------------------------------------------------------------------------
if(APPLE)
    set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "/usr/local/Cellar/mysql-connector-c++/8.0.28")
    include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include)
    link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib)
endif()

# OpenSSL library --------------------------------------------------------------------------------------------
if(APPLE)
    set(OPENSSL_INCLUDE_DIR "/usr/local/Cellar/openssl@1.1/1.1.1m/include")
    set(OPENSSL_CRYPTO_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1m/lib/libcrypto.dylib")
    set(OPENSSL_SSL_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1m/lib/libssl.dylib")
endif()
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    if(APPLE)
        include_directories(/usr/local/Cellar/openssl@1.1/1.1.1m/include)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1m/lib/libssl.dylib)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1m/lib/libcrypto.dylib)
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    endif()
endif()

# ImGui library --------------------------------------------------------------------------------------------
# Set where the ImGui files are stored
set(IMGUI_PATH  "../include/imgui")
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

# ImPlot library --------------------------------------------------------------------------------------------
set(IMPLOT_PATH "../include/implot")

file(GLOB IMPLOT_SOURCES ${IMPLOT_PATH}/*.cpp) 
add_library("ImPlot" STATIC ${IMPLOT_SOURCES})
target_include_directories("ImPlot" PUBLIC ${IMGUI_PATH})

# add the executable
add_executable(Premia Premia.cpp 
                    State/Manager.cpp 
                    State/DemoState.cpp
                    State/PrimaryState.cpp 
                    State/StreamState.cpp
                    State/LoginState.cpp
                    State/Frames/MenuFrame.cpp
                    State/Frames/CandleChartFrame.cpp
                    State/Frames/LinePlotFrame.cpp
                    State/Frames/LoginFrame.cpp
                    State/Frames/View/WatchlistFrame.cpp
                    State/Frames/View/PortfolioFrame.cpp
                    State/Frames/Trade/OptionChainFrame.cpp
                    State/Frames/Tools/ConsoleFrame.cpp
                    State/Frames/Trade/TradingFrame.cpp
                    State/Frames/MarketOverviewFrame.cpp 
                    State/Frames/Analyze/RiskAppetiteFrame.cpp
                    State/Frames/Analyze/FundOwnershipFrame.cpp
                    State/Frames/RiskPremiaFrame.cpp
                    Model/Model.cpp
                    Model/Chart/ChartModel.cpp
                    View/ViewManager.cpp
                    View/PrimaryView.cpp
                    View/Core/LoginView.cpp
                    View/Core/MenuView.cpp
                    View/Dev/DebugView.cpp
                    View/Chart/ChartView.cpp
                    View/Chart/CandleChart.cpp
                    Controller/PrimaryController.cpp
                    Controller/DebugController.cpp
                    Services
                    Services/GenericClient.cpp
                    Services/CoinbasePro/CoinbasePro.cpp
                    Services/CoinbasePro/Account.cpp
                    Services/CoinbasePro/Client.cpp
                    Services/CoinbasePro/Product.cpp
                    Services/TDAmeritrade.cpp 
                    Services/TDAmeritrade/Parser.cpp
                    Services/TDAmeritrade/Session.cpp
                    Services/TDAmeritrade/Client.cpp
                    Services/TDAmeritrade/Data/Quote.cpp 
                    Services/TDAmeritrade/Data/PriceHistory.cpp 
                    Services/TDAmeritrade/Data/OptionChain.cpp 
                    Services/TDAmeritrade/Data/Account.cpp
                    Services/TDAmeritrade/Data/Watchlist.cpp
                    Services/TDAmeritrade/Data/UserPrincipals.cpp
                    Services/TDAmeritrade/Data/Order.cpp
                    Services/IEXCloud.cpp
                    Services/IEXCloud/Client.cpp
                    Services/IEXCloud/Parser.cpp
                    Services/Halext/Halext.cpp
                    Services/Halext/Client.cpp
                    Services/Halext/User.cpp
                    ${IMGUI_PATH}/imgui.cpp
                    ${IMGUI_PATH}/imgui_demo.cpp
                    ${IMGUI_PATH}/imgui_draw.cpp
                    ${IMGUI_PATH}/imgui_widgets.cpp
                    ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
                    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp
                    ${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
                    ${IMPLOT_PATH}/implot.cpp
                    ${IMPLOT_PATH}/implot_items.cpp
                    ${IMPLOT_PATH}/implot_demo.cpp )
include_directories("../assets")

target_link_libraries(Premia ${SDL2_LIBRARIES} 
                             ${SDL_IMAGE_LIBRARIES} 
                             ${SDL_TTF_LIBRARIES}
                             curl OpenSSL::SSL
                             ImGui ImPlot 
                             ${BOOST_LIBRARIES} 
                             ${OPENGL_LIBRARIES} )

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}")
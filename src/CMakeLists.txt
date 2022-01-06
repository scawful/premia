# cmake version
cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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


# OpenSSL library --------------------------------------------------------------------------------------------
if(APPLE)
    set(OPENSSL_INCLUDE_DIR "/usr/local/Cellar/openssl@1.1/1.1.1k/include")
    set(OPENSSL_CRYPTO_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1k/lib/libcrypto.dylib")
    set(OPENSSL_SSL_LIBRARY "/usr/local/Cellar/openssl@1.1/1.1.1k/lib/libssl.dylib")
endif()
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    if(APPLE)
        include_directories(/usr/local/Cellar/openssl@1.1/1.1.1k/include)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1k/lib/libssl.dylib)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1k/lib/libcrypto.dylib)
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    else()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    endif()
endif()

# QuantLib --------------------------------------------------------------------------------------------
#find_package(QuantLib REQUIRED)


# ImGui library --------------------------------------------------------------------------------------------
# Set where the ImGui files are stored
# set(IMGUI_PATH  "${CMAKE_SOURCE_DIR}/Libraries/imgui")
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
# MACOSX_BUNDLE ${MAC_ICON}
add_executable(Premia Premia.cpp 
                    MarketData
                    MarketData/CoinbasePro.cpp
                    MarketData/CoinbasePro/Account.cpp
                    MarketData/CoinbasePro/Client.cpp
                    MarketData/CoinbasePro/Product.cpp
                    MarketData/TDAmeritrade.cpp 
                    MarketData/TDAmeritrade/Session.cpp
                    MarketData/TDAmeritrade/Quote.cpp 
                    MarketData/TDAmeritrade/PriceHistory.cpp 
                    MarketData/TDAmeritrade/OptionChain.cpp 
                    MarketData/TDAmeritrade/Account.cpp
                    State/Manager.cpp 
                    State/DemoState.cpp
                    State/StartState.cpp 
                    State/QuoteState.cpp 
                    State/OptionState.cpp
                    State/StreamState.cpp
                    State/Portfolio/Positions.cpp
                    State/Layout/Menu.cpp
                    State/Frames/MenuFrame.cpp
                    State/Frames/CandleChartFrame.cpp
                    State/Frames/LinePlotFrame.cpp
                    ${IMGUI_PATH}/imgui.cpp
                    ${IMGUI_PATH}/imgui_demo.cpp
                    ${IMGUI_PATH}/imgui_draw.cpp
                    ${IMGUI_PATH}/imgui_widgets.cpp
                    ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
                    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp
                    ${IMPLOT_PATH}/implot.cpp
                    ${IMPLOT_PATH}/implot_items.cpp
                    ${IMPLOT_PATH}/implot_demo.cpp )
include_directories("../assets")

#quantlib library --------------------------------------------------------------------------------------------
# add_library(quantlib STATIC IMPORTED)
# set_property(TARGET quantlib PROPERTY IMPORTED_LOCATION ${QL_LIBRARY})

target_link_libraries(Premia ${SDL2_LIBRARIES} 
                             ${SDL_IMAGE_LIBRARIES} 
                             ${SDL_TTF_LIBRARIES}
                             curl OpenSSL::SSL ImGui ImPlot
                             ${BOOST_LIBRARIES} 
                             ${OPENGL_LIBRARIES} )

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
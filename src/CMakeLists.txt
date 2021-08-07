#Cmake version
cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set the project name
project(Premia VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

# boost library
set(Boost_USE_STATIC_LIBS ON)
# look for package
set(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM "iostreams thread system filesystem") #required by gnuplot-iostream
separate_arguments(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM)

# find the boost libraries
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM})
# find_package(Boost_thread REQUIRED)
# find_package(Boost_system REQUIRED)
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

# find the sdl libraries
find_package(SDL2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)

# find the curl library
find_package(CURL REQUIRED)

find_package(OpenSSL REQUIRED)

# ImGui library --------------------------------------------------------------------------------------------
# Set where the ImGui files are stored
set(IMGUI_PATH  "${CMAKE_SOURCE_DIR}/Libraries/imgui")
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  GL_GLEXT_PROTOTYPES=1)

# add the executable
# MACOSX_BUNDLE ${MAC_ICON}
add_executable(Premia Premia.cpp 
                    MarketData
                    MarketData/CoinbasePro.cpp
                    MarketData/CoinbasePro/Account.cpp
                    MarketData/CoinbasePro/Client.cpp
                    MarketData/TDAmeritrade.cpp 
                    MarketData/TDAmeritrade/Quote.cpp 
                    MarketData/TDAmeritrade/PriceHistory.cpp 
                    MarketData/TDAmeritrade/OptionChain.cpp 
                    MarketData/TDAmeritrade/Account.cpp
                    State/Manager.cpp State/DemoState.cpp
                    State/StartState.cpp State/QuoteState.cpp State/OptionState.cpp
                    State/Layout/Menu.cpp
                    Libraries/imgui/imgui.cpp
                    Libraries/imgui/imgui_demo.cpp
                    Libraries/imgui/imgui_draw.cpp
                    Libraries/imgui/imgui_widgets.cpp
                    Libraries/imgui/imgui_impl_sdl.cpp
                    Libraries/imgui/imgui_sdl.cpp
                    Libraries/imgui/implot.cpp
                    Libraries/imgui/implot_items.cpp
                    Libraries/imgui/implot_demo.cpp )
include_directories("../assets")

#quantlib library --------------------------------------------------------------------------------------------
# add_library(quantlib STATIC IMPORTED)
# set_property(TARGET quantlib PROPERTY IMPORTED_LOCATION ${QL_LIBRARY})

target_link_libraries(Premia SDL2main SDL2 SDL_image SDL_ttf -l:libimm32.a
                             curl OpenSSL::SSL
                             ${BOOST_LIBRARIES} 
                             ${OPENGL_LIBRARIES}
                             ImGui )

set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
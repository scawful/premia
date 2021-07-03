#Cmake version
cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set the project name
project(premia-gui VERSION 1.0)

#set compiler flags based on compiler
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror -Wno-empty-body")
    	# Disabled warning:
    	#   - empty-body: quantlib has empty body for else statements
endif()

if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${warnings}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# add the executable
add_executable(premia-gui Premia.cpp TDAmeritrade.cpp 
                    State/Manager.cpp State/StartState.cpp State/QuoteState.cpp 
                    Graphics/Texture.cpp
                    imgui/imgui.cpp
                    imgui/imgui_demo.cpp
                    imgui/imgui_draw.cpp
                    imgui/imgui_widgets.cpp
                    imgui/imgui_impl_sdl.cpp
                    )
include_directories("../assets")

set(MY_LIBRARY_DIR /usr/local/Cellar)
set(SDL_DIR ${MY_LIBRARY_DIR}/sdl2/2.0.14_1/ )
set(QL_DIR ${MY_LIBRARY_DIR}/quantlib/1.22/ )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

# boost library
set(BOOST_ROOT "/usr/local/include/boost")
# static use of library
set(Boost_USE_STATIC_LIBS TRUE)
# look for package
set(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM "iostreams system filesystem") #required by gnuplot-iostream
separate_arguments(BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM)
find_package(Boost 1.58 REQUIRED COMPONENTS ${BOOST_COMPONENTS_FOR_GNUPLOT_IOSTREAM})
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

include_directories( ${SDL_DIR}/include/SDL2 )
include_directories( ${QL_DIR}/include/ql )

set(SDL_IMAGE_DIR /usr/local/cellar/sdl2_image/2.0.5) 
set(SDL_IMAGE_LIBRARY /usr/local/cellar/sdl2_image/2.0.5/lib/libSDL2_image.dylib) 
set(SDL_IMAGE_INCLUDE_DIR /usr/local/cellar/sdl2_image/2.0.5/include)

set(SDL_TTF_DIR /usr/local/cellar/sdl2_ttf/2.0.15) 
set(SDL_TTF_LIBRARY /usr/local/cellar/sdl2_ttf/2.0.15/lib/libSDL2_ttf.dylib) 
set(SDL_TTF_INCLUDE_DIR /usr/local/cellar/sdl2_ttf/2.0.15/include)

set(QL_LIBRARY /usr/local/Cellar/quantlib/1.22/lib/libQuantLib.dylib)
set(QL_INCLUDE_DIR /usr/local/Cellar/quantlib/1.22/include)

# Set where the ImGui files are stored
set(IMGUI_PATH  "${CMAKE_SOURCE_DIR}/imgui")
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})

# find the sdl library 
find_package(Boost EXACT 1.76.0 REQUIRED)
find_package(Boost_thread REQUIRED)
find_package(Boost_system REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(CURL REQUIRED)

include_directories( ${BOOST_INCLUDE_DIR} )
include_directories( ${SDL2_INCLUDE_DIR} )
include_directories( ${SDL_IMAGE_INCLUDE_DIR} )
include_directories( ${SDL_TTF_INCLUDE_DIR} )
include_directories( ${CURL_INCLUDE_DIR} )

#quantlib library --------------------------------------------------------------------------------------------
add_library(quantlib STATIC IMPORTED)
set_property(TARGET quantlib PROPERTY IMPORTED_LOCATION ${QL_LIBRARY})

target_link_libraries(premia-gui ${BOOST_LIBRARIES} 
                                 ${SDL2_LIBRARIES} 
                                 ${SDL_IMAGE_LIBRARIES} 
                                 ${SDL_TTF_LIBRARIES} 
                                 ${QL_LIBRARY} 
                                 ${CURL_LIBRARIES}
                                 ${OPENGL_LIBRARIES}
                                 ImGui
                                 quantlib )


set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
)
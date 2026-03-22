# Premia Project --------------------------------------------------------------
find_package(Threads)
find_package(CURL REQUIRED)

# ImGui library ---------------------------------------------------------------
set(IMGUI_PATH  "lib/imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_link_libraries("ImGui" PUBLIC SDL2::SDL2)
target_compile_definitions(ImGui PUBLIC
  IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h> GL_GLEXT_PROTOTYPES=1)

# ImPlot library --------------------------------------------------------------
set(IMPLOT_PATH "lib/implot")
file(GLOB IMPLOT_SOURCES ${IMPLOT_PATH}/*.cpp) 
add_library("ImPlot" STATIC ${IMPLOT_SOURCES})
target_include_directories("ImPlot" PUBLIC ${IMGUI_PATH})

include_directories(core/include)
include_directories(providers/include)
include_directories(service)
add_subdirectory(service)
add_subdirectory(providers)
add_subdirectory(core)
add_subdirectory(app)

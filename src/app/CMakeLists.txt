set(
  PREMIA_APP_MODEL_SRC 
  model/model.cc
  model/chart/chart_model.cc
  model/options/options_model.cc
  model/account/account_model.cc
  model/console/console_model.cc
  model/core/watchlist_model.cc
)

set(
  PREMIA_APP_VIEW_SRC
  view/workspace.cc
  view/view_manager.cc
  view/core/primary_view.cc
  view/console/console_view.cc
  view/menu/menu_view.cc
  view/login/login_view.cc
  view/options/option_chain.cc
  view/account/account_view.cc
  view/watchlist/watchlist_view.cc
  view/chart/chart_view.cc
  view/chart/subview/candle_chart.cc
  view/chart/subview/line_plot_chart.cc
)

add_executable(
  premia 
  premia.cc
  core/controller.cc  
  ${PREMIA_APP_MODEL_SRC}
  ${PREMIA_APP_VIEW_SRC}
  ../${IMGUI_PATH}/imgui.cpp 
  ../${IMGUI_PATH}/imgui_demo.cpp
  ../${IMGUI_PATH}/imgui_draw.cpp 
  ../${IMGUI_PATH}/imgui_widgets.cpp
  ../${IMGUI_PATH}/backends/imgui_impl_sdl.cpp
  ../${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp 
  ../${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp
  ../${IMPLOT_PATH}/implot.cpp 
  ../${IMPLOT_PATH}/implot_items.cpp 
  ../${IMPLOT_PATH}/implot_demo.cpp
)

target_include_directories(
  premia PUBLIC
  ../
  ../app
  ../lib
  ../lib/imgui
  ../lib/abseil-cpp
  ../lib/SDL/include
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(SDL_TARGETS SDL2::SDL2)

if(WIN32 OR MINGW)
    list(PREPEND SDL_TARGETS SDL2::SDL2main)
    add_definitions(-DSDL_MAIN_HANDLED)
endif()

target_link_libraries(
  premia
  ${ABSL_TARGETS}
  ${BOOST_LIBRARIES} 
  ${SDL_TARGETS}
  ${OPENGL_LIBRARIES} 
  TWS
  ImGui 
  ImPlot
  curl
  OpenSSL::SSL
  tdameritrade
  coinbase
  interactive_brokers
  halext
)

if (WIN32) 
  target_link_libraries(premia ws2_32)
endif()

# Assets Folder Link ----------------------------------------------------------------------------------------
set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
                   DEPENDS ${destination}
                   COMMENT "symbolic link resources folder from ${source} => ${destination}")

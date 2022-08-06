# cURL library ----------------------------------------------------------------
find_package(CURL REQUIRED)

# ImGui library ---------------------------------------------------------------
set(IMGUI_PATH  "lib/imgui")                         # Set where the ImGui files are stored
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)          # Compile as static library 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC 
  IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  
  GL_GLEXT_PROTOTYPES=1
)

# ImPlot library ----------------------------------------------------------------
set(IMPLOT_PATH "lib/implot")
file(GLOB IMPLOT_SOURCES ${IMPLOT_PATH}/*.cpp) 
add_library("ImPlot" STATIC ${IMPLOT_SOURCES})
target_include_directories("ImPlot" PUBLIC ${IMGUI_PATH})

# TWS library --------------------------------------------------------------------
set(TWS_PATH "lib/tws")
file(GLOB TWS_SOURCES ${TWS_PATH}/*.cpp)
add_library("TWS" STATIC ${TWS_SOURCES})
target_include_directories("TWS" PUBLIC ${TWS_PATH})

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
  view/chart/subview/CandleChart.cpp
  view/chart/subview/LinePlotChart.cpp
)

set(
  PREMIA_SERVICE_TDA_SRC
  service/TDAmeritrade/Parser.cpp
  service/TDAmeritrade/Socket.cpp
  service/TDAmeritrade/Client.cpp
  service/TDAmeritrade/Data/Quote.cpp 
  service/TDAmeritrade/Data/OptionChain.cpp 
  service/TDAmeritrade/Data/Account.cpp
  service/TDAmeritrade/Data/Watchlist.cpp
  service/TDAmeritrade/Data/Order.cpp
  service/TDAmeritrade/Data/UserPrincipals.cpp
  service/TDAmeritrade/Data/PriceHistory.cpp 
)

set(
  PREMIA_SERVICE_CB_SRC
  service/CoinbasePro/Account.cpp 
  service/CoinbasePro/Client.cpp 
  service/CoinbasePro/Product.cpp
)

# Premia Executable Linkage ------------------------------------------------------
add_executable(
  Premia 
  premia.cc
  ${PREMIA_APP_MODEL_SRC}
  ${PREMIA_APP_VIEW_SRC}
  # Controller 
  core/controller.cc    
  # Services 
  service/Halext/Client.cpp 
  service/Halext/User.cpp  
  ## IEXCloud Service  
  service/IEXCloud/Client.cpp 
  service/IEXCloud/Parser.cpp
  ${PREMIA_SERVICE_CB_SRC}
  ${PREMIA_SERVICE_TDA_SRC}
  ## InteractiveBrokers Service
  service/InteractiveBrokers/Client.cpp
  service/InteractiveBrokers/Data/ContractSamples.cpp
  service/InteractiveBrokers/Data/OrderSamples.cpp
  service/InteractiveBrokers/Data/ScannerSubscriptionSamples.cpp
  service/InteractiveBrokers/Data/AvailableAlgoParams.cpp
  service/InteractiveBrokers/Data/AccountSummaryTags.cpp
  service/InteractiveBrokers/Utils.cpp
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

target_include_directories(
  Premia PUBLIC
  .
  lib
  lib/imgui
  lib/SDL/include
)

set(SDL_TARGETS SDL2::SDL2)

if(WIN32 OR MINGW)
    list(PREPEND SDL_TARGETS SDL2::SDL2main)
    add_definitions(-DSDL_MAIN_HANDLED)
endif()

target_link_libraries(
  Premia
  ${BOOST_LIBRARIES} 
  ${SDL_TARGETS}
  ${OPENGL_LIBRARIES} 
  TWS
  ImGui 
  ImPlot
  curl
  OpenSSL::SSL
)

if (WIN32) 
  target_link_libraries(Premia ws2_32)
endif()

# Assets Folder Link ----------------------------------------------------------------------------------------
set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
                   DEPENDS ${destination}
                   COMMENT "symbolic link resources folder from ${source} => ${destination}")

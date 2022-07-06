# SDL libraries ---------------------------------------------------------------------------------------------
find_package(SDL2 REQUIRED)

# cURL library ----------------------------------------------------------------------------------------------
find_package(CURL REQUIRED)

# ImGui library ---------------------------------------------------------------------------------------------
set(IMGUI_PATH  "../libs/imgui")                         # Set where the ImGui files are stored
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp)             # Compile as static library 
add_library("ImGui" STATIC ${IMGUI_SOURCES})
target_include_directories("ImGui" PUBLIC ${IMGUI_PATH})
target_include_directories(ImGui PUBLIC ${SDL2_INCLUDE_DIR})
target_compile_definitions(ImGui PUBLIC 
  IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<SDL2/SDL_opengl.h>  
  GL_GLEXT_PROTOTYPES=1
)

# ImPlot library --------------------------------------------------------------------------------------------
set(IMPLOT_PATH "../libs/implot")
file(GLOB IMPLOT_SOURCES ${IMPLOT_PATH}/*.cpp) 
add_library("ImPlot" STATIC ${IMPLOT_SOURCES})
target_include_directories("ImPlot" PUBLIC ${IMGUI_PATH})

# TWS library -----------------------------------------------------------------------------------------------
set(TWS_PATH "../libs/tws")
file(GLOB TWS_SOURCES ${TWS_PATH}/*.cpp)
add_library("TWS" STATIC ${TWS_SOURCES})
target_include_directories("TWS" PUBLIC ${TWS_PATH})

# Service definitions ---------------------------------------------------------------------------------------
add_subdirectory(Services/TDAmeritrade)

# Premia Executable Linkage ---------------------------------------------------------------------------------
add_executable(
  Premia 
  Premia.cpp 
  # Model
  Model/Model.cpp
  Model/Chart/ChartModel.cpp
  Model/Options/OptionsModel.cpp
  Model/Account/AccountModel.cpp
  Model/Console/ConsoleModel.cpp
  Model/Core/WatchlistModel.cpp
  # View + View Manager 
  View/Workspace.cpp
  View/ViewManager.cpp
  View/Core/PrimaryView.cpp
  View/Console/ConsoleView.cpp
  View/Menu/MenuView.cpp
  View/Login/LoginView.cpp
  View/Options/OptionChain.cpp
  View/Account/AccountView.cpp
  View/Watchlist/WatchlistView.cpp
  View/Chart/ChartView.cpp
  View/Chart/Subview/CandleChart.cpp
  View/Chart/Subview/LinePlotChart.cpp
  # Controller 
  Controller/Controller.cpp    
  # Services 
  Services/Halext/Client.cpp 
  Services/Halext/User.cpp  
  ## IEXCloud Service  
  Services/IEXCloud/Client.cpp 
  Services/IEXCloud/Parser.cpp
  ## Coinbase Service 
  Services/CoinbasePro/Account.cpp 
  Services/CoinbasePro/Client.cpp 
  Services/CoinbasePro/Product.cpp
  ## TDAmeritrade Service 
  Services/TDAmeritrade/Parser.cpp
  Services/TDAmeritrade/Socket.cpp
  Services/TDAmeritrade/Client.cpp
  Services/TDAmeritrade/Data/Quote.cpp 
  Services/TDAmeritrade/Data/OptionChain.cpp 
  Services/TDAmeritrade/Data/Account.cpp
  Services/TDAmeritrade/Data/Watchlist.cpp
  Services/TDAmeritrade/Data/Order.cpp
  Services/TDAmeritrade/Data/UserPrincipals.cpp
  Services/TDAmeritrade/Data/PriceHistory.cpp 
  ## InteractiveBrokers Service
  Services/InteractiveBrokers/Client.cpp
  Services/InteractiveBrokers/Data/ContractSamples.cpp
  Services/InteractiveBrokers/Data/OrderSamples.cpp
  Services/InteractiveBrokers/Data/ScannerSubscriptionSamples.cpp
  Services/InteractiveBrokers/Data/AvailableAlgoParams.cpp
  Services/InteractiveBrokers/Data/AccountSummaryTags.cpp
  Services/InteractiveBrokers/Utils.cpp
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

include_directories(
  Premia 
  "."
  "../assets"
  "../libs"
  "Services"
  "../build/src/Services/TDAmeritrade"
)

target_link_libraries(
  Premia
  ${BOOST_LIBRARIES} 
  ${SDL2_LIBRARIES} 
  ${OPENGL_LIBRARIES} 
  TWS
  ImGui 
  ImPlot
  curl
  OpenSSL::SSL 
  premia_proto
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

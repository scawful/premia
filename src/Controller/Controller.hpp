#ifndef Controller_hpp
#define Controller_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/backends/imgui_impl_sdl.h"
#include "../libs/imgui/backends/imgui_impl_sdlrenderer.h"
#include "../libs/imgui/misc/cpp/imgui_stdlib.h"
#include "../libs/implot/implot.h"
#include "../libs/implot/implot_internal.h"

#include "Premia.hpp"
#include "EventInterface.hpp"
#include "../Model/Model.hpp"
#include "../View/ViewManager.hpp"
#include "../View/Core/LoginView.hpp"
#include "../View/Core/ConsoleView.hpp"
#include "../View/Chart/ChartView.hpp"
#include "../View/Chart/LinePlotChart.hpp"
#include "../View/Options/OptionChain.hpp"

class Controller
{
private:
    bool active = false;
    void quit() {
        active = false;
    }

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;

    Model model;
    ViewManager viewManager;

    void initWindow();
    void initCallbacks();

public:
    explicit Controller(const ViewManager & vm);

    bool isActive() const;

    void onEntry();
    void onInput();
    void onLoad();
    void doRender();
    void onExit();
};

#endif
#ifndef Controller_hpp
#define Controller_hpp

#include "SDL.hpp"
#include "ImGui.hpp"
#include "ImPlot.hpp"
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
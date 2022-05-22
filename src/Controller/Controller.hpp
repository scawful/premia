#ifndef Controller_hpp
#define Controller_hpp

#include "Library/SDL.hpp"
#include "Library/ImPlot.hpp"
#include "Style.hpp"
#include "Model/Model.hpp"
#include "View/ViewManager.hpp"
#include "View/Login/LoginView.hpp"
#include "View/Console/ConsoleView.hpp"
#include "View/Chart/ChartView.hpp"
#include "View/Options/OptionChain.hpp"

class Controller {
private:
    bool active = false;
    void quit() { active = false; }

    Model model;
    ViewManager viewManager;
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;

    void initWindow();
    void initEvents();

public:
    Controller()=default;

    bool isActive() const;

    void onEntry();
    void onInput();
    void onLoad() const;
    void doRender();
    void onExit();
};

#endif
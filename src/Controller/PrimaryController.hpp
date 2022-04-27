#ifndef PrimaryController_hpp
#define PrimaryController_hpp

#include "Controller.hpp"

class PrimaryController : public Controller
{
private:
    int wheel = 0;
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
    PrimaryController(const ViewManager & vm);

    bool isActive() const;

    void onEntry()  override;
    void onInput()  override;
    void onLoad()   override;
    void doRender() override;
    void onExit()   override;
};

#endif
#ifndef DebugController_hpp
#define DebugController_hpp

#include "Controller.hpp"

class DebugController : public Controller 
{
private:
    int wheel = 0;
    bool active = false;

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;

    Model model;
    ViewManager viewManager;
    VoidEventHandler debugCallback;

    void initWindow();
    void initCallbacks();

public:
    DebugController(const ViewManager & vm);

    bool isActive() const;

    void onEntry()  override;
    void onInput()  override;
    void onLoad()   override;
    void doRender() override;
    void onExit()   override;
};

#endif
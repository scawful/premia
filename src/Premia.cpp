#define SDL_MAIN_HANDLED
#include "Controller/PrimaryController.hpp"
#include "Controller/DebugController.hpp"
#include "State/PrimaryState.hpp"
#include "State/LoginState.hpp"

int main(int argc, char *argv[]) 
{
    VoidEventHandler legacyPremia = []() -> void { 
        Manager premia;
        premia.init(300,200);
        premia.change(LoginState::instance());
        while (premia.running()) {
            premia.handleEvents();
            premia.update();
            premia.draw();
        }
        premia.cleanup();
    };

    DebugView debug;
    ViewManager vm(debug, legacyPremia);
    PrimaryModel model;
    DebugController controller(model, vm);

    controller.onEntry();
    while (controller.isActive()) 
    {
        controller.onInput();
        controller.onLoad();
        controller.doRender();
    }
    controller.onExit();
    return 0;
}
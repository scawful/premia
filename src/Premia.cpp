#define SDL_MAIN_HANDLED
#include "Controller/PrimaryController.hpp"
#include "Controller/DebugController.hpp"
#include "State/PrimaryState.hpp"

int main(int argc, char *argv[]) 
{
    VoidEventHandler legacyPremia = []() -> void { 
        Manager premia;
        premia.init(300,200);
        premia.change(PrimaryState::instance());
        while (premia.running()) {
            premia.handleEvents();
            premia.update();
            premia.draw();
        }
        premia.cleanup();
    };

    ViewManager vm;
    PrimaryController controller(vm);

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
#define SDL_MAIN_HANDLED
#include "Controller/Controller.hpp"

int main(int argc, char *argv[]) 
{
    ViewManager vm;
    Controller controller(vm);

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
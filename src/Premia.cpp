#define SDL_MAIN_HANDLED
#include "State/StartState.hpp"

int main(int argc, char *argv[])
{
    Manager premia;

    premia.init();
    premia.change( StartState::instance() );
    
    while ( premia.running() )
    {
        premia.handleEvents();
        premia.update();
        premia.draw();
    }

    // cleanup the engine
    premia.cleanup();

    return 0;
}
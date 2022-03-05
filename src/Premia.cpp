#define SDL_MAIN_HANDLED
#include "State/PrimaryState.hpp"
#include "State/LoginState.hpp"

int main(int argc, char *argv[])
{
    Manager premia;

    premia.init(300,400);
    premia.change( LoginState::instance() );
    
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
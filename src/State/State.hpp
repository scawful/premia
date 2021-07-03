#ifndef State_hpp
#define State_hpp

#include "../core.hpp"
#include "Manager.hpp"

class State
{
public:
    virtual void init( SDL_Renderer *pRenderer, SDL_Window *pWindow ) = 0;
    virtual void cleanup() = 0;
    
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    virtual void handleEvents( Manager *premia ) = 0;
    virtual void update( Manager *premia ) = 0;
    virtual void draw( Manager *premia ) = 0;
    
    void changeState( Manager *premia, State *state )
    {
        premia->change(state);
    }
    
protected:
    State() { }
};

#endif /* State_hpp */

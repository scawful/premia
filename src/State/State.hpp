#ifndef State_hpp
#define State_hpp

#include "../core.hpp"
#include "Manager.hpp"

class State
{
public:
    virtual void init(Manager *premia) = 0;
    virtual void cleanup() = 0;
    
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    virtual void handleEvents() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    
    void changeState(Manager *premia, State *state) const
    {
        premia->change(state);
    }
    
protected:
    State()=default;
    ~State()=default;

};

#endif /* State_hpp */

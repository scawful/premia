//  DemoState Class Header
#ifndef DemoState_hpp
#define DemoState_hpp

#include "State.hpp"

class DemoState : public State
{
private:
    static DemoState m_DemoState;
    
    Manager *premia = NULL;
    SDL_Texture *pTexture = NULL;
        
protected:
    DemoState() { }

public:
    void init(Manager* premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static DemoState* instance()
    {
        return &m_DemoState;
    }

};

#endif /* DemoState_hpp */

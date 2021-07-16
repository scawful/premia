//  DemoState Class Header
#ifndef DemoState_hpp
#define DemoState_hpp

#include "State.hpp"

class DemoState : public State
{
private:
    static DemoState m_DemoState;
    
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    SDL_Texture *pTexture = NULL;
    
    PTexture premiaLogo;

    std::map<std::string, PTexture> textures;
    std::map<std::string, boost::shared_ptr<tda::Quote> > quotes;
    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    
protected:
    DemoState() { }

public:
    void init( SDL_Renderer *zRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();

    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static DemoState* instance()
    {
        return &m_DemoState;
    }

};

#endif /* DemoState_hpp */

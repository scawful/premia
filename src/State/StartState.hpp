//  StartState Class Header
#ifndef StartState_hpp
#define StartState_hpp

#include "State.hpp"

class StartState : public State
{
private:
    static StartState m_StartState;
    
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    SDL_Texture *pTexture = NULL;
    TTF_Font *titleFont = NULL;
    TTF_Font *menuFont = NULL;
    TTF_Font *priceFont = NULL;

    std::map<std::string, PTexture> textures;
    std::map<std::string, boost::shared_ptr<tda::Quote> > quotes;
    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;

    boost::shared_ptr<tda::Account> account_data;
    
protected:
    StartState() { }

public:
    void init( SDL_Renderer *zRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();

    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static StartState* instance()
    {
        return &m_StartState;
    }

};

#endif /* StartState_hpp */

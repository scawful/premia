#ifndef Positions_hpp
#define Positions_hpp

#include "../State.hpp"

class Positions : public State
{
private:
    static Positions m_Positions;
    
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    SDL_Texture *pTexture = NULL;

    std::map<std::string, boost::shared_ptr<tda::Quote> > quotes;
    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    boost::shared_ptr<cbp::CoinbasePro> cbp_data_interface;

    boost::shared_ptr<tda::Account> account_data;
    boost::shared_ptr<cbp::Account> cbp_account_data;
    std::unordered_map<std::string, boost::shared_ptr<cbp::Product> > cbp_products;

    std::vector<std::string> positions_vector;
    

protected:
    Positions() { }

public:
    void init( SDL_Renderer *zRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();

    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static Positions* instance()
    {
        return &m_Positions;
    }

};

#endif /* Positions_hpp */

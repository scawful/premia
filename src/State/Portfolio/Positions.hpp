#ifndef Positions_hpp
#define Positions_hpp

#include "../State.hpp"
#include "../Frames/MenuFrame.hpp"

class Positions : public State
{
private:
    static Positions m_Positions;
    
    Manager *premia = NULL;
    MenuFrame mainMenu;
    SDL_Texture *pTexture = NULL;

    std::vector<std::string> account_ids_std;
    std::vector<const char*> account_ids;
    std::string default_account;

    std::map<std::string, tda::Quote> quotes;
    // boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    // boost::shared_ptr<cbp::CoinbasePro> cbp_data_interface;

    tda::Account account_data;
    // boost::shared_ptr<cbp::Account> cbp_account_data;
    // std::unordered_map<std::string, boost::shared_ptr<cbp::Product> > cbp_products;

    std::vector<std::string> positions_vector;
    
    void load_account( std::string account_num );

protected:
    Positions() { }

public:
    void init( Manager* premia );
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static Positions* instance()
    {
        return &m_Positions;
    }

};

#endif /* Positions_hpp */

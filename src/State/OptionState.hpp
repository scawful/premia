#ifndef OptionState_hpp
#define OptionState_hpp

#include "State.hpp"

class OptionState : public State
{
private:
    static OptionState m_OptionState;

    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRenderer = NULL;
    TTF_Font *tickerFont = NULL;

    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    boost::shared_ptr<tda::OptionChain> optionChainData;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;

protected:
    OptionState() { }

public:
    void init( SDL_Renderer *pRenderer, SDL_Window *pWindow );
    void cleanup();

    void pause();
    void resume();

    void handleEvents( Manager* premia );
    void update( Manager* premia );
    void draw( Manager* premia );

    static OptionState* instance()
    {
        return &m_OptionState;
    }

};

#endif
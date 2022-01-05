#ifndef OptionState_hpp
#define OptionState_hpp

#include "State.hpp"

class OptionState : public State
{
private:
    static OptionState m_OptionState;

    Manager *premia = NULL;
    TTF_Font *tickerFont = NULL;

    boost::shared_ptr<tda::TDAmeritrade> tda_data_interface;
    boost::shared_ptr<tda::OptionChain> optionChainData;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;

protected:
    OptionState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static OptionState* instance()
    {
        return &m_OptionState;
    }

};

#endif
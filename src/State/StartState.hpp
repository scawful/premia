//  StartState Class Header
#ifndef StartState_hpp
#define StartState_hpp

#include "State.hpp"
#include "Frames/MenuFrame.hpp"
#include "Frames/LoginFrame.hpp"
#include "Frames/LinePlotFrame.hpp"
#include "Frames/WatchlistFrame.hpp"
#include "Frames/PortfolioFrame.hpp"
#include "Frames/CandleChartFrame.hpp"
#include "Frames/OptionChainFrame.hpp"
#include "Frames/ConsoleFrame.hpp"
#include "Frames/TradingFrame.hpp"

class StartState : public State
{
private:
    static StartState m_StartState;

    bool is_logged_in = false;
    Manager *premia = NULL;
    SDL_Texture *pTexture = NULL;
    TTF_Font *titleFont = NULL;
    TTF_Font *menuFont = NULL;
    TTF_Font *priceFont = NULL;

    std::string title_string;

    MenuFrame::SubFrame current_frame;

    MenuFrame mainMenu;
    ConsoleFrame console;
    LoginFrame loginFrame;
    LinePlotFrame linePlot;
    PortfolioFrame portfolioFrame;
    WatchlistFrame watchlistFrame;
    CandleChartFrame candleChart;
    OptionChainFrame optionChain;
    TradingFrame tradingFrame;

    std::map<std::string, tda::Quote> quotes;

protected:
    StartState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static StartState* instance()
    {
        return &m_StartState;
    }

};

#endif /* StartState_hpp */

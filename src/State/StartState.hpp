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
#include "Frames/RiskPremiaFrame.hpp"
#include "Frames/MarketOverviewFrame.hpp"
#include "Frames/Trade/TradingFrame.hpp"
#include "Frames/Tools/ConsoleFrame.hpp"
#include "Frames/Trade/OptionChainFrame.hpp"
#include "Frames/Analyze/RiskAppetiteFrame.hpp"
#include "Frames/Analyze/FundOwnershipFrame.hpp"

class StartState : public State
{
private:
    static StartState m_StartState;

    bool protected_mode = true;
    bool tda_logged_in = false;

    Manager *premia = NULL;

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
    MarketOverviewFrame marketOverview;
    RiskAppetiteFrame riskAppetite;
    FundOwnershipFrame fundOwnership;
    RiskPremiaFrame premiaHome;

    std::string title_string;
    std::map<std::string, tda::Quote> quotes;

    void tda_login();

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

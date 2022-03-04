//  PrimaryState Class Header
#ifndef PrimaryState_hpp
#define PrimaryState_hpp

#include "State.hpp"
#include "Frames/MenuFrame.hpp"
#include "Frames/LoginFrame.hpp"
#include "Frames/LinePlotFrame.hpp"
#include "Frames/View/WatchlistFrame.hpp"
#include "Frames/View/PortfolioFrame.hpp"
#include "Frames/CandleChartFrame.hpp"
#include "Frames/RiskPremiaFrame.hpp"
#include "Frames/MarketOverviewFrame.hpp"
#include "Frames/Trade/TradingFrame.hpp"
#include "Frames/Tools/ConsoleFrame.hpp"
#include "Frames/Trade/OptionChainFrame.hpp"
#include "Frames/Analyze/RiskAppetiteFrame.hpp"
#include "Frames/Analyze/FundOwnershipFrame.hpp"

class PrimaryState : public State
{
private:
    static PrimaryState m_PrimaryState;

    Manager* premia = NULL;

    std::string title_string;
    bool protected_mode = true;
    bool tda_logged_in = false;

    // Frames 
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


protected:
    PrimaryState() { }

public:
    void init(Manager *premia);
    void cleanup();

    void pause();
    void resume();

    void handleEvents();
    void update();
    void draw();

    static PrimaryState* instance()
    {
        return &m_PrimaryState;
    }

};

#endif /* PrimaryState_hpp */

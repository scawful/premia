//  PrimaryState Class Header
#ifndef PrimaryState_hpp
#define PrimaryState_hpp

#include "State.hpp"
#include "Frames/MenuFrame.hpp"
#include "Frames/LoginFrame.hpp"
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

    Manager* premia = nullptr;

    std::string title_string;
    bool protected_mode = true;
    bool tda_logged_in = false;

    // Frames 
    MenuFrame::SubFrame current_frame;
    MenuFrame mainMenu;
    ConsoleFrame console;
    LoginFrame loginFrame;
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
    PrimaryState()=default;
    ~PrimaryState()=default;

public:
    void init(Manager *premia) override;
    void cleanup()             override;

    void pause()               override;
    void resume()              override;

    void handleEvents()        override;
    void update()              override;
    void draw()                override;

    static PrimaryState* instance()
    {
        return &m_PrimaryState;
    }

};

#endif /* PrimaryState_hpp */

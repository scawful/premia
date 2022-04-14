#ifndef MenuFrame_hpp
#define MenuFrame_hpp

#include "Frame.hpp"

class MenuFrame : public Frame
{
public:
    enum SubFrame {
        PREMIA_HOME,
        LOGIN,
        MARKET_OVERVIEW,
        LINE_PLOT,
        CANDLE_CHART,
        OPTION_CHAIN,
        TRADING,
        RISK_APPETITE,
        FUND_OWNERSHIP,
        INSIDER_TRANSACTIONS,
        INSTITUTIONAL_OWNERSHIP,
        MARKET_MOVERS
    };

private:
    std::string title_string;
    SubFrame current_frame;
    ImGuiStyle* ref;

    bool portfolio_view;
    bool console_view;
    bool watchlist_view;
    bool free_mode;

    std::unordered_map<std::string, std::string> hot_keys;

    void init_hot_keys();
    void draw_style_editor();

public:
    MenuFrame();

    bool portfolioView() const;
    bool consoleView() const;
    bool watchlistView() const;
    bool freeMode() const;

    void set_title(const std::string & title_string);
    SubFrame get_current_frame() const;
    void update();
    void render();

};


#endif
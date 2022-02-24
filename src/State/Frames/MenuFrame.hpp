#ifndef MenuFrame_hpp
#define MenuFrame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class MenuFrame 
{
public:
    enum SubFrame {
        LOGIN,
        MARKET_OVERVIEW,
        LINE_PLOT,
        CANDLE_CHART,
        OPTION_CHAIN,
        TRADING,
        RISK_APPETITE,
        FUND_OWNERSHIP,
        MARKET_MOVERS
    };

private:
    Manager *premia;
    std::string title_string;
    SubFrame current_frame;
    ImGuiStyle* ref;

    std::unordered_map<std::string, std::string> hot_keys;

    void init_hot_keys();
    void draw_style_editor();

public:
    MenuFrame();

    void import_manager(Manager *premia);
    void set_title(std::string & title_string);
    SubFrame get_current_frame();
    void update();
    void render();

};


#endif
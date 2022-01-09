#ifndef MenuFrame_hpp
#define MenuFrame_hpp

#include "../../core.hpp"
#include "../Manager.hpp"

class MenuFrame 
{
public:
    enum SubFrame {
        LOGIN,
        LINE_PLOT,
        CANDLE_CHART,
        OPTION_CHAIN,
        TRADING
    };

private:
    Manager *premia;
    std::string title_string;
    SubFrame current_frame;
    ImGuiStyle* ref;

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
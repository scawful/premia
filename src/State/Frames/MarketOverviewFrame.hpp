#ifndef MarketOverviewFrame_hpp
#define MarketOverviewFrame_hpp

#include "Frame.hpp"

class MarketOverviewFrame : public Frame
{
private:
    std::unordered_map<std::string, tda::Quote> equity_indice_quotes;
    void init_equity_indice_quotes();

    ImGuiTableFlags equity_indice_flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter |ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    void draw_equity_indice_table();

public:
    MarketOverviewFrame();

    void update();
};

#endif
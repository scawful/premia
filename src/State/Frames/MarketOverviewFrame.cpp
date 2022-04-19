#include "MarketOverviewFrame.hpp"

void MarketOverviewFrame::draw_equity_indice_table()
{
    if (ImGui::BeginTable("Equity Indices", 3, equity_indice_flags)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("SPX");
        ImGui::TableNextColumn();
        ImGui::Text("NDX");
        ImGui::TableNextColumn();
        ImGui::Text("DJI");
        ImGui::EndTable();
    }
}

MarketOverviewFrame::MarketOverviewFrame() : Frame()
{
    
}

void MarketOverviewFrame::update() 
{
    draw_equity_indice_table();
}

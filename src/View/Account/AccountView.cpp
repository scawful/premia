#include "AccountView.hpp"

void AccountView::drawAccountPane()
{
    ImGui::Text("Account ID: " );
    ImGui::Text("Net Liq: ");
    ImGui::Text("Available Funds: ");
    ImGui::Text("Cash: ");
    ImGui::Separator();

    static double xs2[11], ys2[11], v[11];
    for (int i = 0; i < 11; ++i) {
        xs2[i] = i;
        ys2[i] = xs2[i] * xs2[i];
        v[i] = i;
    }
    if (ImPlot::BeginPlot("Account Performance")) {
        ImPlot::PlotBars("Value", v, 11);
        ImPlot::PlotLine("Return", xs2, ys2, 11);
        ImPlot::EndPlot();
    }

    ImGui::Separator();

    /**
     * @brief Button Table
     * 
     */
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings)) {
        for (int i = 0; i < 6; i++) {
            char buf[32];
            sprintf(buf, "%03d", i);
            ImGui::TableNextColumn();
            ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
        }
        ImGui::EndTable();
    }
}

void AccountView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void AccountView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void AccountView::update() 
{
    drawAccountPane();
}

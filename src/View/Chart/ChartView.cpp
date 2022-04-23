#include "ChartView.hpp"

void ChartView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
    if (!ImGui::Begin("Premia", nullptr, flags)) {
        ImGui::End();
        return;
    }

    ImGui::End();
}

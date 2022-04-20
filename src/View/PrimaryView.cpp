#include "PrimaryView.hpp"

void PrimaryView::addEvent(const VoidEventHandler & event) 
{
    this->primaryEvent = event;
}

void PrimaryView::update() 
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("Premia", nullptr, flags)) {
        ImGui::End();
        return;
    }
    ImGui::Text("Primary State");

    ImGui::End();
}

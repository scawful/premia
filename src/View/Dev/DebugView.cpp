#include "DebugView.hpp"

void DebugView::drawScreen() const
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("Debug", nullptr, flags)) {
        ImGui::End();
        return;
    }
    
    if ( ImGui::Button("Legacy State Manager") ) {
        events[0];
    }

    if ( ImGui::Button("New MVC Framework") ) {
        events[1];
    }

    ImGui::End();
}

void DebugView::addEvent(const VoidEventHandler & event) 
{
    this->events.emplace_back(event);
}

void DebugView::update() 
{
    drawScreen();
}

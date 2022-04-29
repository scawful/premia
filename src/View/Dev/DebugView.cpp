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
        events.at("legacy")();
    }

    ImGui::End();
}

void DebugView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void DebugView::addEvent(const std::string & key, const VoidEventHandler & event) 
{
    this->events[key] = event;
}

void DebugView::update() 
{
    drawScreen();
}

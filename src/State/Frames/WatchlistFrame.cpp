#include "WatchlistFrame.hpp"


WatchlistFrame::WatchlistFrame() : Frame()
{
    this->title_string = "Watchlists";    
}

void WatchlistFrame::update() 
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos( ImVec2(io.DisplaySize.x * 0.75, 0) );
    ImGui::SetNextWindowSize( ImVec2(io.DisplaySize.x * 0.25, io.DisplaySize.y * 0.70), ImGuiCond_Always );
    
    if (!ImGui::Begin(  title_string.c_str(), NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::Text("Hello");
    ImGui::End();
}

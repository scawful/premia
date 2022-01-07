#include "OrderFrame.hpp"

OrderFrame::OrderFrame() {
    
}

void OrderFrame::update() 
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos( ImVec2(0, io.DisplaySize.y * 0.75) );
    ImGui::SetNextWindowSize( ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.25), ImGuiCond_Always );
    
    if (!ImGui::Begin("Orders")) {
        ImGui::End();
        return;
    }    

    ImGui::Text("Hello");
    ImGui::End();
}

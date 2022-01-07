#include "LoginFrame.hpp"


LoginFrame::LoginFrame() 
{
    this->isLoggedIn = false;
}

void LoginFrame::update() 
{
    if (!isLoggedIn)
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("TDAmeritrade"))
            {
                static char str0[128] = "";
                ImGui::InputText("Refresh Token", str0, IM_ARRAYSIZE(str0));
                ImGui::Button("Authenticate");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("InteractiveBrokers"))
            {
                ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("CoinbasePro"))
            {
                ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
    }
}

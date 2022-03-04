#include "LoginFrame.hpp"

LoginFrame::LoginFrame() : Frame()
{
    this->isLoggedIn = false;
    this->clientId = 0;
    this->connectOptions = "";
    this->port = 7497;
}

void LoginFrame::update() 
{
    ImGui::Text("Select your broker/exchange API to use account features like trading and portfolio analysis.");

    static char host_char[128] = "127.0.0.1";
    if (!isLoggedIn)
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("TDAmeritrade"))
            {
                ImGui::Spacing();
                static char api_key[128] = "";
                static char refresh_token[128] = "";
                ImGui::InputText("Consumer Key", api_key, IM_ARRAYSIZE(api_key));
                ImGui::InputText("Refresh Token", refresh_token, IM_ARRAYSIZE(refresh_token));
                ImGui::Button("Login");
                ImGui::SameLine();
                if (ImGui::Button("Fetch from file")) {
                    *tda_logged_in = true;
                    *public_mode = false;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("IEXCloud"))
            {
                ImGui::Spacing();
                static char api_key[128] = "";
                ImGui::InputText("API Key", api_key, IM_ARRAYSIZE(api_key));
                ImGui::Button("Login");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("CoinbasePro"))
            {
                ImGui::Spacing();
                static char api_key[128] = "";
                static char secret_key[128] = "";
                static char passphrase[128] = "";
                ImGui::InputText("API Key", api_key, IM_ARRAYSIZE(api_key));
                ImGui::InputText("Secret Key", secret_key, IM_ARRAYSIZE(secret_key));
                ImGui::InputText("Passphrase", passphrase, IM_ARRAYSIZE(passphrase));
                ImGui::Button("Login");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

}

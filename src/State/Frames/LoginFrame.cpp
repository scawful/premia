#include "LoginFrame.hpp"

const unsigned SLEEP_TIME = 10;
const unsigned MAX_ATTEMPTS = 50;

LoginFrame::LoginFrame() : Frame()
{
    this->ibkr_doConnect = false;
    this->isLoggedIn = false;
    this->clientId = 0;
    this->connectOptions = "";
    this->port = 7497;
}

void LoginFrame::update() 
{

    ImGui::Text("Welcome to Premia!");
    ImGui::Text("Select your broker/exchange API to use account features like trading and portfolio analysis.");

    static char host_char[128] = "127.0.0.1";
    if (!isLoggedIn)
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("TDAmeritrade"))
            {
                ImGui::Text("By default, Premia comes with its own protected development key.");
                ImGui::Text("Upon inserting your own API key, the program will be released from protected mode and restarted");
                static char api_key[128] = "";
                static char refresh_token[128] = "";
                ImGui::InputText("API Key", api_key, IM_ARRAYSIZE(api_key));
                ImGui::InputText("Refresh Token", refresh_token, IM_ARRAYSIZE(refresh_token));
                ImGui::Button("Authenticate");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("InteractiveBrokers"))
            {
                if (port <= 0)
                    port = 7496;
                
                ImGui::Text("WARNING: Program will crash without Trader Workstation or Gateway application open.");
                ImGui::InputText("Host", host_char, IM_ARRAYSIZE(host_char));
                ImGui::InputInt("Port", &port);

                if (ImGui::Button("Connect")) {
                    this->ibkr_doConnect = true;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("CoinbasePro"))
            {
                static char api_key[128] = "";
                static char secret_key[128] = "";
                static char passphrase[128] = "";
                ImGui::InputText("API Key", api_key, IM_ARRAYSIZE(api_key));
                ImGui::InputText("Secret Key", secret_key, IM_ARRAYSIZE(secret_key));
                ImGui::InputText("Passphrase", passphrase, IM_ARRAYSIZE(passphrase));
                ImGui::Button("Login");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("KuCoin"))
            {
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
    }

    if (ibkr_doConnect) 
    {
        for (;;) {
            ++attempt;
            printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS);
            const char* host = "127.0.0.1";
            // Run time error will occur (here) if TestCppClient.exe is compiled in debug mode but TwsSocketClient.dll is compiled in Release mode
            // TwsSocketClient.dll (in Release Mode) is copied by API installer into SysWOW64 folder within Windows directory 
            
            // if( connectOptions ) {
            //     premia->ibkr_client.setConnectOptions(connectOptions);
            // }
            
            premia->ibkr_client.connect( host, port, clientId );
            
            while( premia->ibkr_client.isConnected()) {
                premia->ibkr_client.processMessages();
            }
            if( attempt >= MAX_ATTEMPTS) {
                break;
            }

            printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
            std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));
	    }

    }

}

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
    static char host_char[128] = "127.0.0.1";
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
                if (port <= 0)
                    port = 7496;
                
                ImGui::InputText("Host", host_char, IM_ARRAYSIZE(host_char));
                ImGui::InputInt("Port", &port);

                if (ImGui::Button("Connect")) {
                    this->ibkr_doConnect = true;
                }
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

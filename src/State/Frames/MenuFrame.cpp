#include "MenuFrame.hpp"
#include "../OptionState.hpp"
#include "../StartState.hpp"
#include "../StreamState.hpp"

MenuFrame::MenuFrame()
{

}

void MenuFrame::import_manager(Manager *premia) 
{
    this->premia = premia;
}

void MenuFrame::set_title(std::string &title_string)
{
    this->title_string = title_string;
}

void MenuFrame::update()
{
    static bool about = false;
    ImGui::NewFrame();
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize( ImVec2(io.DisplaySize.x * 0.75, io.DisplaySize.y * 0.75), ImGuiCond_Always );
    
    if (!ImGui::Begin(  title_string.c_str(), NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( title_string != "Home" )
            {
                if ( ImGui::MenuItem("Return Home") )
                {
                    premia->change( StartState::instance() );
                }
                ImGui::Separator();
            }

            if ( ImGui::MenuItem("New Instance", "CTRL + N") )
            {
                premia->change( StartState::instance() );
            }

            if ( ImGui::MenuItem("Authenticate") )
            {
                // premia->tda_client->manual_authentication();
            }

            if ( ImGui::MenuItem("Start WS Session") )
            {
                premia->tda_client.start_session();
            }

            
            ImGui::Separator();
            if (ImGui::BeginMenu("Options"))
            {
                static bool enabled = true;
                ImGui::MenuItem("Private Balances", "", &enabled);

                ImGui::BeginChild("child", ImVec2(0, 60), true);
                for (int i = 0; i < 10; i++)
                    ImGui::Text("Scrolling Text %d", i);
                ImGui::EndChild();

                static float f = 0.5f;
                static int n = 0;
                ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                ImGui::InputFloat("Input", &f, 0.1f);

                ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
                ImGui::EndMenu();
            }

            if ( ImGui::MenuItem("Quit", "ESC") )
            {
                premia->quit();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Portfolio"))
        {

            if ( ImGui::MenuItem("Positions") )
            {
                // premia->change( Positions::instance() );
            }

            if ( ImGui::MenuItem("Balances") )
            {
                // premia->change( BalancesState::instance() );
            }

            if ( ImGui::MenuItem("Orders" ) )
            {
                // premia->change( OrderState::instance() );
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Trade"))
        {
            if ( ImGui::MenuItem("Place Order") )
            {

            }

            if ( ImGui::MenuItem("Replace Order") )
            {
                // replace order
            }

            if ( ImGui::MenuItem("Cancel Order") )
            {
                // cancel order
            }

            ImGui::Separator();

            if ( ImGui::MenuItem("Get Order") )
            {
                // get order 
            }
         
            if ( ImGui::MenuItem("Option Chain") )
            {
                premia->change( OptionState::instance() );
            }


            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu("Charts"))
        {

            if ( ImGui::MenuItem("Live Quotes") )
            {
                premia->change( StreamState::instance() );
            }

            if ( ImGui::MenuItem("Static Quotes") )
            {
                // premia->change( QuoteState::instance() );
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Analyze"))
        {
            ImGui::MenuItem("Money Flows");
            ImGui::MenuItem("Volatility Cycles");

            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("tools menu bar");
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu("Help") )
        {
            if ( ImGui::MenuItem("About") )
                about = true;

            ImGui::EndMenu();
        }


        ImGui::EndMenuBar();
    }

    if ( about )
        ImGui::OpenPopup("About");

    if ( ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        ImGui::Text("Premia Version 0.01");
        ImGui::Text("Written by: Justin Scofield (scawful)");
        ImGui::Text("Dependencies: SDL2, SDL_image, SDL_ttf, OpenGL");
        ImGui::Text("\t\t\t  Boost, OpenSSL, libcURL, QuantLib");
        ImGui::Text("\t\t\t  ImGui, ImGuiSDL, ImPlot");
        ImGui::Text("API: TDAmeritrade, Coinbase Pro");

        if ( ImGui::Button("Close", ImVec2(120, 0)) ) { about = false; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}
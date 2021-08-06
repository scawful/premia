#include "Menu.hpp"
#include "../QuoteState.hpp"
#include "../OptionState.hpp"
#include "../StartState.hpp"
// #include "../PortfolioState.hpp"

void draw_imgui_menu( Manager *premia, boost::shared_ptr<tda::TDAmeritrade> tda_data_interface, std::string title_string )
{
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Users\\starw\\Code\\premia\\assets\\SourceSansPro-Black.ttf", 12.0f );
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
    
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
                tda_data_interface->manual_authentication();
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

            if ( ImGui::MenuItem("Quit", "CTRL + Q") )
            {
                premia->quit();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Portfolio"))
        {
            if ( ImGui::MenuItem("Orders" ) )
            {
                // premia->change( OrderState::instance() );
            }
            if ( ImGui::MenuItem("Positions") )
            {
                // premia->change( PositionsState::instance() );
            }

            if ( ImGui::MenuItem("Balances") )
            {
                // premia->change( BalancesState::instance() );
            }

            if ( ImGui::MenuItem("Allocation") )
            {
                // premia->change( AllocationState::instance() );
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

            ImGui::EndMenu();
        }
        if ( ImGui::BeginMenu("Analyze"))
        {
            if ( ImGui::MenuItem("Instrument Quote") )
            {
                premia->change( QuoteState::instance() );
            }
            
            if ( ImGui::MenuItem("Option Chain") )
            {
                premia->change( OptionState::instance() );
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Research"))
        {
            ImGui::MenuItem("research menu bar");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("tools menu bar");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

}
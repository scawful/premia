#include "TradingFrame.hpp"

TradingFrame::TradingFrame() : Frame()
{
    
}

void TradingFrame::update() 
{
   // Place Order Button ---------------------------------------------------------------------------------
    if ( ImGui::Button("Quick Order", ImVec2(120, 30)) )
        ImGui::OpenPopup("Quick Order");

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if ( ImGui::BeginPopupModal("Quick Order", NULL, ImGuiWindowFlags_AlwaysAutoResize) )
    {
        static std::string new_ticker = "";
        static bool active_instrument = false;
        static char buf[64] = "";
        ImGui::InputText("##symbol", buf, 64, ImGuiInputTextFlags_CharsUppercase );
        ImGui::SameLine(); 
        if ( ImGui::Button("Search") )
        {
            if ( strcmp(buf, "") != 0 )
            {
                new_ticker = std::string(buf);
                std::cout << "new ticker " << new_ticker << std::endl;
                quotes[ new_ticker ] = premia->tda_client.createQuote( new_ticker );
                active_instrument = true;
            }
            else
            {
                active_instrument = false;
            }
        }

        if ( active_instrument )
        {
            static int n = 0;
            ImGui::Combo("Order Type", &n, "Limit\0Market\0Stop\0Stop Limit\0\0");

            ImGui::Text("%s - %s", quotes[new_ticker].getQuoteVariable("symbol").c_str(), quotes[new_ticker].getQuoteVariable("description").c_str() );
            ImGui::Text("Bid: %s - Size: %s", quotes[new_ticker].getQuoteVariable("bidPrice").c_str(), quotes[new_ticker].getQuoteVariable("bidSize").c_str() );
            ImGui::Text("Ask: %s - Size: %s", quotes[new_ticker].getQuoteVariable("askPrice").c_str(), quotes[new_ticker].getQuoteVariable("askSize").c_str() );
            ImGui::Separator();

            static float order_quantity = 10.f;
            ImGui::InputFloat("Quanitity", &order_quantity, 10.f);

            double security_cost = 0.0;
            std::string bidPrice = quotes[new_ticker].getQuoteVariable("bidPrice");
            security_cost = std::stod( bidPrice );
            security_cost *= order_quantity;
            ImGui::Text("Cost: %lf", security_cost );
        }

        ImGui::Separator();
        ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

        if (ImGui::Button("Confirm", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    } 
}

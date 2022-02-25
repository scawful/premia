#include "FundOwnershipFrame.hpp"


FundOwnershipFrame::FundOwnershipFrame() 
{
    this->data_active = false;   
}

void FundOwnershipFrame::update() 
{
    ImGui::Text("Fund Ownership");

    static char symbol[128] = "";
    ImGui::InputText("Enter symbol", symbol, IM_ARRAYSIZE(symbol));

    if ( ImGui::Button("Submit") ) 
    {
        fund_ownership = premia->iex_client.getFundOwnership(symbol);
        data_active = true;
    }

    if ( data_active ) 
    {
        ImGui::Text("Symbol: %s", fund_ownership[0].getSymbol().c_str());
        

        if (ImGui::BeginTable("table1", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Fund Name");
            ImGui::TableSetupColumn("Adj Holding");
            ImGui::TableSetupColumn("Adj Market Value");
            ImGui::TableSetupColumn("Reported Holding");
            ImGui::TableSetupColumn("Reported Market Value");
            ImGui::TableSetupColumn("Report Date");
            ImGui::TableHeadersRow();

            for ( auto & each : fund_ownership ) 
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", each.entityProperName().c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", each.adjustedHoldings().c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", each.adjustedMarketValue().c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", each.reportedHolding().c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%s", each.reportedMarketValue().c_str());
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%ld", each.reportDate());
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

}

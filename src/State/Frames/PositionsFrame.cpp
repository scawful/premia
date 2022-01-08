#include "PositionsFrame.hpp"


PositionsFrame::PositionsFrame() : Frame()
{

}

void PositionsFrame::init_positions()
{
    account_ids_std = premia->tda_client.get_all_accounts();
    for ( std::string const& each_id : account_ids_std ) {
        account_ids.push_back(each_id.c_str());
    }
    default_account = account_ids_std.at(0);
    load_account(default_account);
}

void PositionsFrame::load_account( std::string account_num )
{
    account_data = premia->tda_client.createAccount( account_num );

    if ( positions_vector.size() != 0 ) {
        positions_vector.clear();
    }

    for ( int i = 0; i < account_data.get_position_vector_size(); i++ )
    {
        for ( auto& position_it : account_data.get_position( i ) )
        {
            if ( position_it.first == "symbol" )
            {
                std::string str = position_it.second;
                positions_vector.push_back( str );
            }
        }
    }
}

void PositionsFrame::update() 
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos( ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.70) );
    ImGui::SetNextWindowSize( ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.30), ImGuiCond_Always );

    if (!ImGui::Begin("Portfolio")) {
        ImGui::End();
        return;
    }    

     // Load Account IDs
    static int n = 0;
    const char **accounts = account_ids.data();
    if ( ImGui::Button( "Change Account" ) ) {
        load_account( accounts[n] );
    } 
    ImGui::SameLine();
    ImGui::Combo("Accounts", &n,  accounts, 6); 
    
    // ImGui::Combo("Order Type", &n, "Limit\0Market\0Stop\0Stop Limit\0\0");
    ImGui::Text("TDAmeritrade Portfolio Information");
    ImGui::Separator();
    ImGui::Text( "Account ID: %s", account_data.get_account_variable("accountId").c_str() );
    ImGui::Text( "Net Liq: %s", account_data.get_balance_variable("liquidationValue").c_str() );
    ImGui::Text( "Available Funds: %s", account_data.get_balance_variable("availableFunds").c_str() );
    ImGui::Text( "Cash: %s", account_data.get_balance_variable("cashBalance").c_str() );

    ImGui::Separator();
    ImGui::Text("Positions");
    ImGui::Separator();
    ImGui::Spacing();

    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 15);

    if (ImGui::BeginTable("table_scrolly", 6, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableSetupColumn("P/L Day", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("P/L %", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Avg Price",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Mkt Value",  ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( positions_vector.size() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 6; column++)
                {
                    std::string symbol = positions_vector[row];
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "currentDayProfitLoss" ).c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "currentDayProfitLossPercentage").c_str() );
                            break;
                        case 3:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "averagePrice" ).c_str());
                            break;
                        case 4:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "marketValue" ).c_str());
                            break;
                        case 5:
                            ImGui::Text("%s", account_data.get_position_balances( symbol, "longQuantity" ).c_str());
                            break;
                        default:
                            ImGui::Text("Hello %d,%d", column, row);
                            break;
                    }
                }
            }
        }

        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::End();
}

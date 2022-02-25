#include "WatchlistFrame.hpp"

void WatchlistFrame::init_watchlists()
{
    std::string account_num = premia->tda_interface.get_all_accounts().at(0);
    watchlists = premia->tda_interface.retrieveWatchlistsByAccount(account_num);
    for ( int i = 0; i < watchlists.size(); i++ )
    {
        watchlist_names.push_back(watchlists[i].getName());
    }
    for (std::string const& str : watchlist_names) {
        watchlist_names_char.push_back(str.data());
    }
}

void WatchlistFrame::draw_watchlist_table()
{
    static int n = 0;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * watchlists[n].getNumInstruments());

    const char **watchlist_names_gui = watchlist_names_char.data();
    ImGui::SameLine();
    ImGui::Combo(" ", &n,  watchlist_names_gui, watchlist_names.size()); 

    if (ImGui::BeginTable("Watchlist_Table", 3, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( watchlists[n].getNumInstruments() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    std::string symbol = watchlists[n].getInstrumentSymbol(row);
                    std::string description = watchlists[n].getInstrumentDescription(row);
                    std::string assetType = watchlists[n].getInstrumentType(row);
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", description.c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", assetType.c_str() );
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
}

void WatchlistFrame::draw_custom_watchlist_table()
{
    static int n = 0;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 10);

    static std::string new_instrument = "";
    ImGui::InputText("Add", &new_instrument);
    std::vector<tda::Watchlist::WatchlistInstrument> instruments;

    if (ImGui::BeginTable("Watchlist_Table", 3, flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( instruments.size() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    std::string symbol = instruments[n].getSymbol();
                    std::string description = instruments[n].getDescription();
                    std::string assetType = instruments[n].getType();
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", description.c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", assetType.c_str() );
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
}

WatchlistFrame::WatchlistFrame() : Frame()
{
    this->title_string = "Watchlists";  
}

void WatchlistFrame::update() 
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos( ImVec2(io.DisplaySize.x * 0.75, 0) );
    ImGui::SetNextWindowSize( ImVec2(io.DisplaySize.x * 0.25, io.DisplaySize.y * 0.70), ImGuiCond_Always );
    
    if (!ImGui::Begin(  title_string.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (*tda_logged_in) {
        draw_watchlist_table();
    } else {
        draw_custom_watchlist_table();
    }

    ImGui::End();
}

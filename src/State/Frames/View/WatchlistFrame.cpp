#include "WatchlistFrame.hpp"

void WatchlistFrame::init_watchlists()
{
    std::string account_num = premia->tda_interface.get_all_accounts().at(0);
    watchlists = premia->tda_interface.getWatchlistsByAccount(account_num);
    openList = new bool[watchlists.size()];

    for ( int i = 0; i < watchlists.size(); i++ ) {
        watchlist_names.push_back(watchlists[i].getName());
        openList[i] = false;
    }

    for (std::string const& str : watchlist_names) {
        watchlist_names_char.push_back(str.data());
    }

    setInitialized(true);
}

void WatchlistFrame::draw_watchlist_table()
{
    static int n = 0;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    ImGui::SameLine();
    ImGui::Combo(" ", &n,  watchlist_names_char.data(), (int) watchlist_names.size()); 

    if (openList[n] == false) {
        for (int j = 0; j < watchlists[n].getNumInstruments(); j++) {
           quotes[watchlists[n].getInstrumentSymbol(j)] = premia->tda_interface.getQuote(watchlists[n].getInstrumentSymbol(j));
        }
        openList[n] = true;
    }

    if (ImGui::BeginTable("Watchlist_Table", 5, flags, ImGui::GetContentRegionAvail()))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( watchlists[n].getNumInstruments() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    std::string symbol = watchlists[n].getInstrumentSymbol(row);
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", quotes[symbol].getQuoteVariable("bidPrice").c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", quotes[symbol].getQuoteVariable("askPrice").c_str());
                            break;
                        case 3:
                            ImGui::Text("%s", quotes[symbol].getQuoteVariable("openPrice").c_str());
                            break;
                        case 4:
                            ImGui::Text("%s", quotes[symbol].getQuoteVariable("closePrice").c_str());
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
    setInitialized(false);
}

WatchlistFrame::~WatchlistFrame()=default;

void WatchlistFrame::update() 
{
    
    if (!ImGui::Begin(  title_string.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (true) {
        if (!getInitialized()) {
            init_watchlists();
        }
        draw_watchlist_table();
    } else {
        draw_custom_watchlist_table();
    }

    ImGui::End();
}

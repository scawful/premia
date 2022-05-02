#include "WatchlistView.hpp"

void WatchlistView::drawWatchlistTable()
{
    if (!ImGui::Begin("Watchlists", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar )) {
        ImGui::End();
        return;
    }

    static int n = 0;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    ImGui::SameLine();
    ImGui::Combo(" ", &n,  model.getWatchlistNamesCharVec().data(), (int) model.getWatchlistNamesCharVec().size()); 

    if (model.getOpenList(n) == 0) {
        for (int j = 0; j < model.getWatchlist(n).getNumInstruments(); j++) {
           model.setQuote(model.getWatchlist(n).getInstrumentSymbol(j), model.getTDAInterface().getQuote(model.getWatchlist(n).getInstrumentSymbol(j)));
        }
        model.setOpenList(n);
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
        clipper.Begin( model.getWatchlist(n).getNumInstruments() );
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    String symbol = model.getWatchlist(n).getInstrumentSymbol(row);
                    ImGui::TableSetColumnIndex(column);
                    switch( column )
                    {
                        case 0:
                            ImGui::Text("%s", symbol.c_str() );
                            break;
                        case 1:
                            ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("bidPrice").c_str());
                            break;
                        case 2:
                            ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("askPrice").c_str());
                            break;
                        case 3:
                            ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("openPrice").c_str());
                            break;
                        case 4:
                            ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("closePrice").c_str());
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

void WatchlistView::drawCustomWatchlistTable()
{
    static int n = 0;
    static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 10);

    static String new_instrument = "";
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
                    String symbol = instruments[n].getSymbol();
                    String description = instruments[n].getDescription();
                    String assetType = instruments[n].getType();
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

void WatchlistView::addAuth(CRString key , CRString token)
{
    model.addAuth(key, token);
}

void WatchlistView::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void WatchlistView::addEvent(CRString key, const Premia::EventHandler & event)
{
    this->events[key] = event;
}

void WatchlistView::update() 
{
    if (!model.isActive()) {
        model.addLogger(this->logger);
        model.initWatchlist();
    }
    drawWatchlistTable();
}

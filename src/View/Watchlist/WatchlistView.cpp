#include "WatchlistView.hpp"

void WatchlistView::drawWatchlistTable()
{
    static int n = 5;

    ImGui::Combo(" ", &n,  model.getWatchlistNamesCharVec().data(), (int) model.getWatchlistNamesCharVec().size()); 

    if (model.getOpenList(n) == 0) {
        for (int j = 0; j < model.getWatchlist(n).getNumInstruments(); j++) {
            model.setQuote(model.getWatchlist(n).getInstrumentSymbol(j), tda::TDA::getInstance().getQuote(model.getWatchlist(n).getInstrumentSymbol(j)));
        }
        model.setOpenList(n);
    }

    if (ImGui::BeginTable("Watchlist_Table", 5, watchlistFlags, ImVec2(0,0))) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin( model.getWatchlist(n).getNumInstruments() );
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++) {
                    String symbol = model.getWatchlist(n).getInstrumentSymbol(row);
                    ImGui::TableSetColumnIndex(column);
                    switch (column) {
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
                            break;
                    }
                }
            }
        }

        ImGui::EndTable();
    }
}

String 
WatchlistView::getName() {
    return "Watchlists";
}

void WatchlistView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void WatchlistView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void WatchlistView::update() 
{
    if (!model.isActive() && !isInit) {
        model.addLogger(this->logger);
        try {
            model.initWatchlist();
            isLoggedIn = true;
        } catch (Premia::NotLoggedInException) {
            logger("User not logged in, loading empty watchlist pane");
        } 
        isInit = true;
    }
    
    if (isLoggedIn) {
        drawWatchlistTable();
    } else {
        ImGui::Text("empty watchlist pane goes here");
    }

}

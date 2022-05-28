#include "WatchlistView.hpp"
namespace Premia {
void WatchlistView::drawWatchlistMenu() {
  const char* names[] = {
      "Local",
      "TDAmeritrade",
      "Coinbase",
  };
  static bool toggles[] = {true, false, false};

  if (ImGui::Button("Service")) ImGui::OpenPopup("service_popup");

  ImGui::SameLine();
  ImGui::TextUnformatted(names[currentService]);
  if (ImGui::BeginPopup("service_popup")) {
    if (ImGui::Selectable(names[0])) currentService = 0;
    if (ImGui::Selectable(names[1])) currentService = 1;
    if (ImGui::Selectable(names[2])) currentService = 2;
    ImGui::EndPopup();
  }
}

void WatchlistView::drawWatchlistTable() {
  static int n = 0;

  ImGui::Combo("##watchlists", &n, model.getWatchlistNamesCharVec().data(),
               (int)model.getWatchlistNamesCharVec().size());

  if (model.getOpenList(n) == 0) {
    for (int j = 0; j < model.getWatchlist(n).getNumInstruments(); j++) {
      model.setQuote(model.getWatchlist(n).getInstrumentSymbol(j),
                     tda::TDA::getInstance().getQuote(
                         model.getWatchlist(n).getInstrumentSymbol(j)));
    }
    model.setOpenList(n);
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MD_EDIT_NOTE)) ImGui::OpenPopup("edit_popup");
  if (ImGui::BeginPopup("edit_popup")) {
    ImGui::Button(ICON_MD_ADD);
    ImGui::Button(ICON_MD_DELETE);
    ImGui::EndPopup();
  }

  if (ImGui::BeginTable("Watchlist_Table", 5, watchlistFlags, ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin(model.getWatchlist(n).getNumInstruments());
    while (clipper.Step()) {
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
        ImGui::TableNextRow();
        for (int column = 0; column < 5; column++) {
          String symbol = model.getWatchlist(n).getInstrumentSymbol(row);
          ImGui::TableSetColumnIndex(column);
          switch (column) {
            case 0:
              ImGui::Text("%s", symbol.c_str());
              break;
            case 1:
              ImGui::Text(
                  "%s",
                  model.getQuote(symbol).getQuoteVariable("bidPrice").c_str());
              break;
            case 2:
              ImGui::Text(
                  "%s",
                  model.getQuote(symbol).getQuoteVariable("askPrice").c_str());
              break;
            case 3:
              ImGui::Text(
                  "%s",
                  model.getQuote(symbol).getQuoteVariable("openPrice").c_str());
              break;
            case 4:
              ImGui::Text("%s", model.getQuote(symbol)
                                    .getQuoteVariable("closePrice")
                                    .c_str());
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

String WatchlistView::getName() { return "Watchlists"; }

void WatchlistView::addLogger(const Logger& newLogger) {
  this->logger = newLogger;
}

void WatchlistView::addEvent(CRString key, const EventHandler& event) {
  this->events[key] = event;
}

void WatchlistView::update() {
  Construct {  // runs once
    model.addLogger(this->logger);
  }
  Instruct {  // runs each frame
    drawWatchlistMenu();
    switch (currentService) {
      case 0:
        ImGui::Text("Local Watchlist");
        break;
      case 1:
        Construct {
          try {
            model.initTDAWatchlists();
          } catch (Premia::NotLoggedInException) {
            logger("[Watchlist] User not logged in!");
            throw Destruction();
          }
        }
        Instruct { drawWatchlistTable(); }
        RecursiveDestruct;
        break;
      case 2:
        ImGui::Text("Coinbase Watchlist");
        break;
      default:
        break;
    }
  }
  Destruct {  // runs on throw Destruction, can be used for errors and memory
              // managment
              // user is not logged into a service
    // perhaps make a popup window that directs them to log into that service?
    // (not necessarily an error)
  }
  Proceed;
}
}  // namespace Premia
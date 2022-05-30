#include "WatchlistView.hpp"

namespace Premia {
void
WatchlistView::drawWatchlistMenu() {
  const int sz = 3;
  const char *names[] = {"Local", "TDAmeritrade", "Coinbase",};
  static bool toggles[] = {true, false, false};

  if (ImGui::Button("Service"))
    ImGui::OpenPopup("service_popup");

  ImGui::SameLine();
  ImGui::TextUnformatted(names[currentService]);
  if (ImGui::BeginPopup("service_popup")) {
    for (int i = 0; i < sz; i++) {
      if (ImGui::Selectable(names[i])) {
        if (currentService != i) {
          serviceChanged = true;
        }
        currentService = i;
      }
    }
    ImGui::EndPopup();
  }
}

void
WatchlistView::drawWatchlistTable() {
  ImGui::Combo("##watchlists",
               &watchlistIndex,
               model.getWatchlistNamesCharVec().data(),
               (int) model.getWatchlistNamesCharVec().size());

  if (model.getOpenList(watchlistIndex) == 0) {
    for (int j = 0; j < model.getWatchlist(watchlistIndex).getNumInstruments(); j++) {
      model.setQuote(model.getWatchlist(watchlistIndex).getInstrumentSymbol(j), //TODO: handle for local responses
                     tda::TDA::getInstance().getQuote(model.getWatchlist(watchlistIndex).getInstrumentSymbol(j)));
    }
    model.setOpenList(watchlistIndex);
  }

  static String addText;
  ImGui::SameLine();
  if (ImGui::Button(ICON_MD_EDIT_NOTE))
    ImGui::OpenPopup("edit_popup");
  if (ImGui::BeginPopup("edit_popup")) {
    // If add button pressed, render the textbox
    if (ImGui::Button(ICON_MD_ADD)) {
      ImGui::OpenPopup("add_popup");
    }
    if (ImGui::BeginPopup("add_popup")) {
      if (ImGui::BeginTable("addTable", 2, ImGuiTableFlags_None)) {
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(175.f);
        if (ImGui::InputText("##addText", &addText, ImGuiInputTextFlags_EnterReturnsTrue)) {
          printf("enter pressed?\n");
          fflush(stdout);
          // TODO: Insert implementation
          // TODO: Save to file
        }
        ImGui::TableNextColumn();
        ImGui::Text(ICON_MD_SEARCH);
        ImGui::EndTable();
      }
      ImGui::EndPopup();
    }
    ImGui::Separator();
    ImGui::Button(ICON_MD_DELETE);
    ImGui::EndPopup();
  }

  static int selectedRow = -1;
  if (ImGui::BeginTable("Watchlist_Table", 5, watchlistFlags, ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Open", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Close", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    ImGuiListClipper clipper;
    clipper.Begin(model.getWatchlist(watchlistIndex).getNumInstruments());
    while (clipper.Step()) {
      bool selected = false;
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
        String symbol = model.getWatchlist(watchlistIndex).getInstrumentSymbol(row);
        auto symChar = symbol.c_str();
        ImGui::TableNextRow();
        for (int column = 0; column < 5; column++) {
          ImGui::TableSetColumnIndex(column);
          switch (column) {
            case 0:
              // Left click
              if (ImGui::Selectable(symChar,
                                    &selected,
                                    ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups)) {
                printf("Selected %s\n", symChar);
                fflush(stdout);
              }
              // Right click
              if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                printf("Right click-Selected %s\n", symChar);
                fflush(stdout);
                ImGui::OpenPopup("rowSelectPopup");
                selectedRow = row;
              }

              break;
            case 1:ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("bidPrice").c_str());
              break;
            case 2:ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("askPrice").c_str());
              break;
            case 3:ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("openPrice").c_str());
              break;
            case 4:ImGui::Text("%s", model.getQuote(symbol).getQuoteVariable("closePrice").c_str());
              break;
            default:break;
          }
        }

      }
    }

    // Right click entry menu
    if (ImGui::BeginPopupContextItem("rowSelectPopup")) {
      if (ImGui::MenuItem("Delete") && selectedRow != -1) {
        String symbol = model.getWatchlist(watchlistIndex).getInstrumentSymbol(selectedRow);
        auto symChar = symbol.c_str();
        printf("delete-Selected %s\n", symChar);
        fflush(stdout);
        // TODO: Delete implementation given watchlistIndex, selectedRow
        // TODO: Save to file
        selectedRow = -1;
      }
      ImGui::EndPopup();
    }
    ImGui::EndTable();
  }
}

String
WatchlistView::getName() {
  return "Watchlists";
}

void
WatchlistView::addLogger(const Logger &newLogger) {
  this->logger = newLogger;
}

void
WatchlistView::addEvent(CRString key, const EventHandler &event) {
  this->events[key] = event;
}

void WatchlistView::update() {
  Construct { // runs once
              model.addLogger(this->logger);
            }Instruct { // runs each frame
              drawWatchlistMenu();
              switch (currentService) {
                case 0:
                  // ImGui::Text("Local Watchlist");
                  if (serviceChanged) {
                    // Reset variables
                    watchlistIndex = 0;
                    serviceChanged = false;
                    try {
                      model.initWatchlist();
                    } catch (Premia::NotLoggedInException) {
                      logger("[Watchlist] No local watchlist data found!");
                      throw Destruction();
                    }
                  } else {
                    drawWatchlistTable();
                  }
                  break;
                case 1:
                  if (serviceChanged) {
                    // Reset variables
                    watchlistIndex = 0;
                    serviceChanged = false;
                    try {
                      model.initTDAWatchlists();
                    } catch (Premia::NotLoggedInException) {
                      logger("[Watchlist] User not logged in!");
                      throw Destruction();
                    }
                  } else {
                    drawWatchlistTable();
                  }
                  break;
                case 2: ImGui::Text("Coinbase Watchlist");
                  break;
                default: break;
              }
            }Destruct { // runs on throw Destruction, can be used for errors and memory managment
              // user is not logged into a service
              // perhaps make a popup window that directs them to log into that service? (not necessarily an error)
            }
  Proceed;
}
}

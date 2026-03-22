#include "watchlist_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "premia/core/application/composition_root.hpp"
#include "premia/core/application/scaffold_application_service.hpp"
#include "view/core/IconsMaterialDesign.h"

namespace premia {

void WatchlistView::DrawWatchlistMenu() {
  const int sz = 2;
  const char *names[] = {
      "Local",
      "TDAmeritrade",
  };

  if (ImGui::Button("Service")) ImGui::OpenPopup("service_popup");

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

void WatchlistView::DrawWatchlistTable() {
  ImGui::Combo("##watchlists", &watchlistIndex,
               model.getWatchlistNamesCharVec().data(),
               (int)model.getWatchlistNamesCharVec().size());

  if (model.getOpenList(watchlistIndex) == 0) {
    for (int j = 0; j < model.getWatchlist(watchlistIndex).getNumInstruments();
         j++) {
      model.setQuote(
          model.getWatchlist(watchlistIndex).getInstrumentSymbol(j),
          tda::TDA::getInstance().getQuote(
              model.getWatchlist(watchlistIndex).getInstrumentSymbol(j)));
    }
    model.setOpenList(watchlistIndex);
  }

  static std::string addText;
  ImGui::SameLine();

  // Edit watchlist button/menu/popup thing
  if (ImGui::Button(ICON_MD_EDIT_NOTE)) {
    ImGui::OpenPopup("edit_watchlist_popup");
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Edit watchlists");
  }

  if (ImGui::BeginPopup("edit_watchlist_popup")) {
    // If add button pressed, render the textbox
    if (ImGui::Button(ICON_MD_ADD)) {
      ImGui::OpenPopup("add_watchlist_popup");
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Add watchlist");
    }

    if (ImGui::BeginPopup("add_watchlist_popup")) {
      if (ImGui::InputTextWithHint("##addText", "Enter watchlist name",
                                   &addText,
                                   ImGuiInputTextFlags_EnterReturnsTrue)) {
        boost::to_upper(addText);
        addText = "";
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
    ImGui::Separator();
    if (ImGui::Button(ICON_MD_DELETE)) {
      ImGui::SetTooltip("Delete watchlist");
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Delete watchlist");
    }
    ImGui::EndPopup();
  }

  // Button for adding entry to watchlist
  ImGui::SameLine();
  if (ImGui::Button(ICON_MD_ADD)) ImGui::OpenPopup("add_entry_popup");
  if (ImGui::BeginPopup("add_entry_popup")) {
    if (ImGui::InputTextWithHint("##addText", "Enter ticker", &addText,
                                 ImGuiInputTextFlags_EnterReturnsTrue)) {
      boost::to_upper(addText);
      if (!model.getWatchlist(watchlistIndex).containsTicker(addText)) {
        model.getWatchlist(watchlistIndex).addInstrument(addText, "", "Stock");
        int rIdx = model.getWatchlist(watchlistIndex).getNumInstruments() - 1;
        model.setQuote(
            model.getWatchlist(watchlistIndex).getInstrumentSymbol(rIdx),
            tda::TDA::getInstance().getQuote(
                model.getWatchlist(watchlistIndex).getInstrumentSymbol(rIdx)));
      }
      addText = "";
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Add entry");
  }

  static int selectedRow = -1;
  if (ImGui::BeginTable("Watchlist_Table", 5, watchlistFlags, ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
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
        std::string symbol =
            model.getWatchlist(watchlistIndex).getInstrumentSymbol(row);
        auto symChar = symbol.c_str();
        ImGui::TableNextRow();
        for (int column = 0; column < 5; column++) {
          ImGui::TableSetColumnIndex(column);
          switch (column) {
            case 0:
              // Left click
              if (ImGui::Selectable(symChar, &selected,
                                    ImGuiSelectableFlags_SpanAllColumns |
                                        ImGuiSelectableFlags_DontClosePopups)) {
              }
              if (ImGui::IsItemHovered() &&
                  ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup("rowSelectPopup");
                selectedRow = row;
              }

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

    // Right click entry menu
    if (ImGui::BeginPopupContextItem("rowSelectPopup")) {
      if (ImGui::MenuItem("Delete") && selectedRow != -1) {
        model.setOpenList(watchlistIndex, 0);
        model.getWatchlist(watchlistIndex).removeInstrument(selectedRow);
        selectedRow = -1;
      }
      ImGui::EndPopup();
    }
    ImGui::EndTable();
  }
}

void WatchlistView::DrawCoreWatchlistPreview() {
  auto& service = core::application::CompositionRoot::Instance().AppService();
  const auto watchlists = service.ListWatchlists();
  if (watchlists.empty()) {
    ImGui::TextDisabled("No normalized watchlists available.");
    return;
  }

  if (watchlistIndex >= static_cast<int>(watchlists.size())) {
    watchlistIndex = 0;
  }

  std::vector<const char*> watchlist_names;
  watchlist_names.reserve(watchlists.size());
  for (const auto& watchlist : watchlists) {
    watchlist_names.push_back(watchlist.name.c_str());
  }

  ImGui::Text("Core Watchlist Preview");
  ImGui::TextDisabled(
      "This fallback table is driven by premia_core screen contracts.");
  ImGui::Separator();

  ImGui::Combo("##core_watchlists", &watchlistIndex, watchlist_names.data(),
               static_cast<int>(watchlist_names.size()));

  const auto screen = service.GetWatchlistScreen(watchlists[watchlistIndex].id);
  if (ImGui::BeginTable("CoreWatchlistTable", 5,
                        watchlistFlags | ImGuiTableFlags_Borders,
                        ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    for (const auto& row : screen.rows) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%s", row.symbol.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", row.name.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("$%s", row.last_price.amount.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("$%s", row.bid.amount.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("$%s", row.ask.amount.c_str());
    }

    ImGui::EndTable();
  }

  ImGui::Separator();
  ImGui::Text("Connections");
  for (const auto& connection : service.GetConnections()) {
    ImGui::BulletText("%s - %s", connection.display_name.c_str(),
                      core::domain::ConnectionStatusToString(connection.status)
                          .c_str());
  }
}

std::string WatchlistView::getName() { return "Watchlists"; }

void WatchlistView::addLogger(const Logger &newLogger) {
  this->logger = newLogger;
}

void WatchlistView::addEvent(const std::string &key,
                             const EventHandler &event) {
  this->events[key] = event;
}

void WatchlistView::Update() {
  CONSTRUCT {  // runs once
    model.addLogger(this->logger);
  }
  INSTRUCT {  // runs each frame
    DrawWatchlistMenu();
    switch (currentService) {
      case 0:
        if (serviceChanged) {
          watchlistIndex = 0;
          serviceChanged = false;
          try {
            model.initLocalWatchlist();
            isLoggedIn = true;
          } catch (premia::NotLoggedInException) {
            logger("[Watchlist] No local watchlist data found!");
            isLoggedIn = false;
          }
        }

        if (isLoggedIn) {
          DrawWatchlistTable();
        } else {
          DrawCoreWatchlistPreview();
        }
        break;
      case 1:
        if (serviceChanged) {
          watchlistIndex = 0;
          serviceChanged = false;
          try {
            model.initTDAWatchlists();
            isLoggedIn = true;
          } catch (premia::NotLoggedInException) {
            logger("[Watchlist] User not logged in!");
            isLoggedIn = false;
          }
        }

        if (isLoggedIn) {
          DrawWatchlistTable();
        } else {
          DrawCoreWatchlistPreview();
        }
        break;
      default:
        break;
    }
  }
  DESTRUCT {  // runs on throw Destruction, can be used for errors and memory
              // managment
    DrawCoreWatchlistPreview();
  }
  PROCEED;
}
}  // namespace premia

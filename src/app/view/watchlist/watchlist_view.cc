#include "watchlist_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>
#include <vector>

#include "premia/core/application/composition_root.hpp"
#include "view/core/IconsMaterialDesign.h"

namespace premia {

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
  DrawCoreWatchlistPreview();
}
}  // namespace premia

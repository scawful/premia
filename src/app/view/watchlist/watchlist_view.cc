#include "watchlist_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "premia/core/application/composition_root.hpp"
#include "view/core/IconsMaterialDesign.h"

namespace premia {

namespace {

auto DayChangeColor(const std::string& amount) -> ImVec4 {
  try {
    return std::stod(amount) < 0.0 ? ImVec4(0.89f, 0.34f, 0.36f, 1.0f)
                                    : ImVec4(0.24f, 0.78f, 0.55f, 1.0f);
  } catch (...) {
    return ImVec4(0.58f, 0.63f, 0.71f, 1.0f);
  }
}

auto ToLower(std::string value) -> std::string {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return value;
}

auto MatchesFilter(const premia::core::application::WatchlistRow& row,
                   const std::string& filter_text, int movement_filter) -> bool {
  if (!filter_text.empty()) {
    const auto symbol = ToLower(row.symbol);
    const auto name = ToLower(row.name);
    const auto query = ToLower(filter_text);
    if (symbol.find(query) == std::string::npos &&
        name.find(query) == std::string::npos) {
      return false;
    }
  }

  double change = 0.0;
  try {
    change = std::stod(row.day_change.absolute.amount.empty()
                           ? "0.00"
                           : row.day_change.absolute.amount);
  } catch (...) {
    change = 0.0;
  }
  if (movement_filter == 1 && change < 0.0) {
    return false;
  }
  if (movement_filter == 2 && change > 0.0) {
    return false;
  }
  return true;
}

auto SpreadText(const premia::core::application::WatchlistRow& row) -> std::string {
  try {
    const auto bid = std::stod(row.bid.amount);
    const auto ask = std::stod(row.ask.amount);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << (ask - bid);
    return "$" + oss.str();
  } catch (...) {
    return "--";
  }
}

}  // namespace

void WatchlistView::DrawWatchlistSummary(
    const core::application::WatchlistScreenData& screen) {
  int visible_rows = 0;
  int positive_rows = 0;
  int negative_rows = 0;
  for (const auto& row : screen.rows) {
    if (!MatchesFilter(row, filter_text_, movement_filter_)) {
      continue;
    }
    ++visible_rows;
    try {
      const auto change = std::stod(row.day_change.absolute.amount);
      if (change >= 0.0) {
        ++positive_rows;
      } else {
        ++negative_rows;
      }
    } catch (...) {
    }
  }

  if (ImGui::BeginTable("WatchlistSummaryTable", 4,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    ImGui::BeginChild("VisibleRowsCard", ImVec2(0.0f, 78.0f), true);
    ImGui::TextDisabled("Visible Symbols");
    ImGui::Text("%d", visible_rows);
    ImGui::TextDisabled("Selected: %s",
                        selected_symbol_.empty() ? "none" : selected_symbol_.c_str());
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("AdvancersCard", ImVec2(0.0f, 78.0f), true);
    ImGui::TextDisabled("Advancers");
    ImGui::TextColored(ImVec4(0.24f, 0.78f, 0.55f, 1.0f), "%d", positive_rows);
    ImGui::TextDisabled("Currently filtered rows");
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("DeclinersCard", ImVec2(0.0f, 78.0f), true);
    ImGui::TextDisabled("Decliners");
    ImGui::TextColored(ImVec4(0.89f, 0.34f, 0.36f, 1.0f), "%d", negative_rows);
    ImGui::TextDisabled("Currently filtered rows");
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("WatchlistNameCard", ImVec2(0.0f, 78.0f), true);
    ImGui::TextDisabled("Watchlist");
    ImGui::TextWrapped("%s", screen.watchlist.name.c_str());
    ImGui::TextDisabled("Universe size: %d", screen.watchlist.instrument_count);
    ImGui::EndChild();

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
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f),
                     "Normalized Watchlists");
  ImGui::TextDisabled(
      "This surface uses shared watchlist contracts and currently falls back to local or transitional sources.");
  ImGui::Separator();

  ImGui::Combo("##core_watchlists", &watchlistIndex, watchlist_names.data(),
               static_cast<int>(watchlist_names.size()));

  const auto screen = service.GetWatchlistScreen(watchlists[watchlistIndex].id);
  DrawWatchlistSummary(screen);
  ImGui::Spacing();

  ImGui::InputTextWithHint("##watchlistFilter", "Filter by symbol or name",
                           &filter_text_);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(160.0f);
  ImGui::Combo("##watchlistMovementFilter", &movement_filter_,
               "All Moves\0Advancers\0Decliners\0");

  if (ImGui::BeginTable("CoreWatchlistTable", 8,
                        watchlistFlags | ImGuiTableFlags_Borders,
                        ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Day", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Spread", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Updated", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    for (const auto& row : screen.rows) {
      if (!MatchesFilter(row, filter_text_, movement_filter_)) {
        continue;
      }
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      const bool is_selected = selected_symbol_ == row.symbol;
      if (ImGui::Selectable(row.symbol.c_str(), is_selected,
                            ImGuiSelectableFlags_SpanAllColumns)) {
        selected_symbol_ = row.symbol;
        if (symbol_selection_handler_) {
          symbol_selection_handler_(row.symbol);
        }
      }
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%s", row.name.c_str());
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("$%s", row.last_price.amount.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::TextColored(DayChangeColor(row.day_change.absolute.amount),
                         "$%s (%s%%)", row.day_change.absolute.amount.c_str(),
                         row.day_change.percent.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::Text("$%s", row.bid.amount.c_str());
      ImGui::TableSetColumnIndex(5);
      ImGui::Text("$%s", row.ask.amount.c_str());
      ImGui::TableSetColumnIndex(6);
      ImGui::Text("%s", SpreadText(row).c_str());
      ImGui::TableSetColumnIndex(7);
      ImGui::TextDisabled("%s", row.updated_at.empty() ? "-" : row.updated_at.c_str());
    }

    ImGui::EndTable();
  }

  ImGui::Separator();
  ImGui::Text("Connections");
  for (const auto& connection : service.GetConnections()) {
    if (connection.provider == core::domain::Provider::kSchwab) {
        ImGui::TextColored(
            ImVec4(0.40f, 0.72f, 0.96f, 1.0f), "%s - %s (Market Data)",
            connection.display_name.c_str(),
            core::domain::ConnectionStatusToString(connection.status).c_str());
      } else {
      ImGui::BulletText("%s - %s", connection.display_name.c_str(),
                        core::domain::ConnectionStatusToString(connection.status)
                            .c_str());
    }
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

void WatchlistView::SetSelectedSymbol(const std::string& symbol) {
  selected_symbol_ = symbol;
}

void WatchlistView::SetSymbolSelectionHandler(
    const std::function<void(const std::string&)>& handler) {
  symbol_selection_handler_ = handler;
}

void WatchlistView::Update() {
  DrawCoreWatchlistPreview();
}
}  // namespace premia

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

auto MatchesAnySymbol(const std::vector<core::application::WatchlistRow>& rows,
                      const std::string& symbol) -> bool {
  return std::any_of(rows.begin(), rows.end(), [&symbol](const auto& row) {
    return row.symbol == symbol;
  });
}

}  // namespace

void WatchlistView::LoadState() {
  state_loaded_ = true;
}

void WatchlistView::PersistState() const {}

void WatchlistView::EnsureWatchlistOrdering(
    const core::application::WatchlistScreenData&) {}

auto WatchlistView::BuildOrderedRows(
    const core::application::WatchlistScreenData& screen) const
    -> std::vector<core::application::WatchlistRow> {
  return screen.rows;
}

auto WatchlistView::IsPinned(const std::string& watchlist_id,
                             const std::string& symbol) const -> bool {
  const auto screen = core::application::CompositionRoot::Instance()
                          .AppService()
                          .GetWatchlistScreen(watchlist_id);
  const auto it = std::find_if(screen.rows.begin(), screen.rows.end(),
                               [&symbol](const core::application::WatchlistRow& row) {
                                 return row.symbol == symbol;
                               });
  return it != screen.rows.end() && it->is_pinned;
}

void WatchlistView::TogglePinned(const std::string& watchlist_id,
                                 const std::string& symbol) {
  auto& service = core::application::CompositionRoot::Instance().AppService();
  const auto screen = service.GetWatchlistScreen(watchlist_id);
  const auto it = std::find_if(screen.rows.begin(), screen.rows.end(),
                               [&symbol](const core::application::WatchlistRow& row) {
                                 return row.symbol == symbol;
                               });
  const bool next_state = it == screen.rows.end() ? true : !it->is_pinned;
  service.PinWatchlistSymbol(watchlist_id, symbol, next_state);
  status_message_ = std::string(next_state ? "Pinned " : "Unpinned ") + symbol + ".";
}

void WatchlistView::MoveSymbolBefore(const std::string& watchlist_id,
                                     const std::string& symbol,
                                     const std::string& before_symbol) {
  if (symbol == before_symbol) {
    return;
  }
  core::application::CompositionRoot::Instance()
      .AppService()
      .MoveWatchlistSymbol(watchlist_id, symbol, before_symbol);
  status_message_ = "Moved " + symbol + " within the current watchlist.";
}

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

void WatchlistView::DrawComparePanel(
    const core::application::WatchlistScreenData& primary,
    const core::application::WatchlistScreenData& secondary,
    const std::vector<core::application::WatchlistRow>& primary_rows,
    const std::vector<core::application::WatchlistRow>& secondary_rows) {
  int overlap_count = 0;
  for (const auto& row : primary_rows) {
    if (MatchesAnySymbol(secondary_rows, row.symbol)) {
      ++overlap_count;
    }
  }

  ImGui::BeginChild("WatchlistComparePanel", ImVec2(0.0f, 300.0f), true);
  ImGui::Text("Watchlist Compare");
  ImGui::TextDisabled("Primary: %s | Secondary: %s | Overlap: %d",
                      primary.watchlist.name.c_str(), secondary.watchlist.name.c_str(),
                      overlap_count);

  if (ImGui::BeginTable("WatchlistCompareTables", 2,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    const auto draw_side = [&](const char* id,
                               const core::application::WatchlistScreenData& screen_data,
                               const std::vector<core::application::WatchlistRow>& rows,
                               bool highlight_overlap) {
      ImGui::BeginChild(id, ImVec2(0.0f, 240.0f), true);
      ImGui::Text("%s", screen_data.watchlist.name.c_str());
      ImGui::Separator();
      if (ImGui::BeginTable((std::string(id) + "Table").c_str(), 4,
                            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("Symbol");
        ImGui::TableSetupColumn("Last");
        ImGui::TableSetupColumn("Day");
        ImGui::TableSetupColumn("Flag");
        ImGui::TableHeadersRow();
        for (const auto& row : rows) {
          if (!MatchesFilter(row, filter_text_, movement_filter_)) {
            continue;
          }
          const bool overlap = highlight_overlap && MatchesAnySymbol(
              highlight_overlap ? secondary_rows : primary_rows, row.symbol);
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%s", row.symbol.c_str());
          ImGui::TableSetColumnIndex(1);
          ImGui::Text("$%s", row.last_price.amount.c_str());
          ImGui::TableSetColumnIndex(2);
          ImGui::TextColored(DayChangeColor(row.day_change.absolute.amount),
                             "%s%%", row.day_change.percent.c_str());
          ImGui::TableSetColumnIndex(3);
          if (row.is_pinned) {
            ImGui::TextColored(ImVec4(0.92f, 0.78f, 0.36f, 1.0f), "Pinned");
          } else if (overlap) {
            ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f), "Overlap");
          } else {
            ImGui::TextDisabled("-");
          }
        }
        ImGui::EndTable();
      }
      ImGui::EndChild();
    };

    ImGui::TableNextColumn();
    draw_side("PrimaryCompareSide", primary, primary_rows, true);
    ImGui::TableNextColumn();
    draw_side("SecondaryCompareSide", secondary, secondary_rows, true);
    ImGui::EndTable();
  }

  ImGui::EndChild();
}

void WatchlistView::DrawCoreWatchlistPreview() {
  LoadState();
  auto& service = core::application::CompositionRoot::Instance().AppService();
  const auto all_watchlists = service.ListWatchlists();
  std::vector<core::application::WatchlistSummary> watchlists;
  for (const auto& watchlist : all_watchlists) {
    if (!show_archived_ && watchlist.is_archived) {
      continue;
    }
    watchlists.push_back(watchlist);
  }
  if (watchlists.empty()) {
    ImGui::TextDisabled("No normalized watchlists available.");
    ImGui::Checkbox("Show archived", &show_archived_);
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

  ImGui::Checkbox("Show archived", &show_archived_);
  ImGui::SameLine();
  ImGui::TextDisabled("Archived watchlists stay available for later restore or delete.");

  ImGui::Combo("##core_watchlists", &watchlistIndex, watchlist_names.data(),
               static_cast<int>(watchlist_names.size()));

  const auto screen = service.GetWatchlistScreen(watchlists[watchlistIndex].id);
  if (rename_watchlist_name_.empty() || rename_watchlist_name_ == screen.watchlist.name) {
    rename_watchlist_name_ = screen.watchlist.name;
  }
  EnsureWatchlistOrdering(screen);
  const auto rows = BuildOrderedRows(screen);

  std::vector<core::application::WatchlistSummary> compare_targets;
  for (const auto& watchlist : watchlists) {
    if (watchlist.id != screen.watchlist.id) {
      compare_targets.push_back(watchlist);
    }
  }
  if (compare_watchlist_index_ >= static_cast<int>(compare_targets.size())) {
    compare_watchlist_index_ = 0;
  }

  std::vector<core::application::WatchlistSummary> transfer_targets;
  for (const auto& watchlist : all_watchlists) {
    if (watchlist.id != screen.watchlist.id && !watchlist.is_archived) {
      transfer_targets.push_back(watchlist);
    }
  }
  if (destination_watchlist_index_ >= static_cast<int>(transfer_targets.size())) {
    destination_watchlist_index_ = 0;
  }

  if (ImGui::BeginTable("WatchlistMutationBar", 5,
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV)) {
    ImGui::TableNextColumn();
    ImGui::InputTextWithHint("##newWatchlistName", "New watchlist name",
                             &new_watchlist_name_);
    if (ImGui::Button("Create Watchlist", ImVec2(-FLT_MIN, 0.0f)) &&
        !new_watchlist_name_.empty()) {
      try {
        const auto summary = service.CreateWatchlist(new_watchlist_name_);
        new_watchlist_name_.clear();
        status_message_ = "Created watchlist " + summary.name + ".";
        const auto refreshed = service.ListWatchlists();
        for (int index = 0; index < static_cast<int>(refreshed.size()); ++index) {
          if (refreshed[index].id == summary.id) {
            watchlistIndex = index;
            break;
          }
        }
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist create failed: ") + ex.what();
      }
    }

    ImGui::TableNextColumn();
    ImGui::InputTextWithHint("##renameWatchlistName", "Rename current watchlist",
                             &rename_watchlist_name_);
    if (ImGui::Button("Rename", ImVec2(-FLT_MIN, 0.0f)) &&
        !rename_watchlist_name_.empty()) {
      try {
        const auto summary =
            service.RenameWatchlist(screen.watchlist.id, rename_watchlist_name_);
        status_message_ = "Renamed watchlist to " + summary.name + ".";
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist rename failed: ") + ex.what();
      }
    }

    ImGui::TableNextColumn();
    ImGui::InputTextWithHint("##newSymbolInput", "Add symbol to current watchlist",
                             &new_symbol_input_, ImGuiInputTextFlags_CharsUppercase);
    if (ImGui::Button("Add Symbol", ImVec2(-FLT_MIN, 0.0f)) && !new_symbol_input_.empty()) {
      try {
        service.AddWatchlistSymbol(screen.watchlist.id, new_symbol_input_);
        status_message_ = "Added " + new_symbol_input_ + " to " +
                          screen.watchlist.name + ".";
        new_symbol_input_.clear();
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist add failed: ") + ex.what();
      }
    }

    ImGui::TableNextColumn();
    if (ImGui::Button("Remove Selected", ImVec2(-FLT_MIN, 0.0f)) &&
        !selected_symbol_.empty()) {
      try {
        service.RemoveWatchlistSymbol(screen.watchlist.id, selected_symbol_);
        status_message_ = "Removed " + selected_symbol_ + " from " +
                          screen.watchlist.name + ".";
        selected_symbol_.clear();
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist remove failed: ") + ex.what();
      }
    }

    ImGui::TableNextColumn();
    if (!transfer_targets.empty()) {
      std::vector<const char*> target_labels;
      target_labels.reserve(transfer_targets.size());
      for (const auto& target : transfer_targets) {
        target_labels.push_back(target.name.c_str());
      }
      ImGui::Combo("##moveWatchlistTarget", &destination_watchlist_index_,
                   target_labels.data(), static_cast<int>(target_labels.size()));
      if (ImGui::Button("Move Selected To", ImVec2(-FLT_MIN, 0.0f)) &&
          !selected_symbol_.empty()) {
        try {
          service.MoveSymbolToWatchlist(screen.watchlist.id,
                                        transfer_targets[destination_watchlist_index_].id,
                                        selected_symbol_);
          status_message_ = "Moved " + selected_symbol_ + " to " +
                            transfer_targets[destination_watchlist_index_].name + ".";
          selected_symbol_.clear();
        } catch (const std::exception& ex) {
          status_message_ = std::string("Watchlist move failed: ") + ex.what();
        }
      }
    } else {
      ImGui::TextDisabled("No transfer targets");
    }

    ImGui::TableNextColumn();
    const auto archive_label = screen.watchlist.is_archived ? "Restore Watchlist"
                                                            : "Archive Watchlist";
    if (ImGui::Button(archive_label, ImVec2(-FLT_MIN, 0.0f))) {
      try {
        const auto summary =
            service.ArchiveWatchlist(screen.watchlist.id, !screen.watchlist.is_archived);
        status_message_ = std::string(summary.is_archived ? "Archived " : "Restored ") +
                          summary.name + ".";
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist archive failed: ") + ex.what();
      }
    }
    if (screen.watchlist.is_archived &&
        ImGui::Button("Delete Archived", ImVec2(-FLT_MIN, 0.0f))) {
      try {
        const auto summary = service.DeleteWatchlist(screen.watchlist.id);
        status_message_ = "Deleted archived watchlist " + summary.name + ".";
        watchlistIndex = 0;
        selected_symbol_.clear();
      } catch (const std::exception& ex) {
        status_message_ = std::string("Watchlist delete failed: ") + ex.what();
      }
    }
    ImGui::EndTable();
  }

  if (!status_message_.empty()) {
    ImGui::TextDisabled("%s", status_message_.c_str());
  }

  if (!compare_targets.empty()) {
    ImGui::Checkbox("Compare Watchlists", &compare_mode_);
    if (compare_mode_) {
      ImGui::SameLine();
      std::vector<const char*> compare_names;
      compare_names.reserve(compare_targets.size());
      for (const auto& watchlist : compare_targets) {
        compare_names.push_back(watchlist.name.c_str());
      }
      ImGui::SetNextItemWidth(220.0f);
      ImGui::Combo("##compareWatchlist", &compare_watchlist_index_,
                   compare_names.data(), static_cast<int>(compare_names.size()));
      const auto secondary =
          service.GetWatchlistScreen(compare_targets[compare_watchlist_index_].id);
      const auto secondary_rows = BuildOrderedRows(secondary);
      DrawComparePanel(screen, secondary, rows, secondary_rows);
      ImGui::Spacing();
    }
  }

  DrawWatchlistSummary(screen);
  ImGui::Spacing();

  ImGui::InputTextWithHint("##watchlistFilter", "Filter by symbol or name",
                           &filter_text_);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(160.0f);
  ImGui::Combo("##watchlistMovementFilter", &movement_filter_,
               "All Moves\0Advancers\0Decliners\0");

  if (ImGui::BeginTable("CoreWatchlistTable", 9,
                        watchlistFlags | ImGuiTableFlags_Borders,
                        ImVec2(0, 0))) {
    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Pin", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Last", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Day", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Bid", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Ask", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Spread", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Updated", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableHeadersRow();

    for (const auto& row : rows) {
      if (!MatchesFilter(row, filter_text_, movement_filter_)) {
        continue;
      }
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      const auto pin_label = row.is_pinned ? "*" : ".";
      if (ImGui::SmallButton((std::string(pin_label) + "##pin_" + row.symbol).c_str())) {
        TogglePinned(screen.watchlist.id, row.symbol);
      }
      ImGui::TableSetColumnIndex(1);
      const bool is_selected = selected_symbol_ == row.symbol;
      if (ImGui::Selectable(row.symbol.c_str(), is_selected,
                            ImGuiSelectableFlags_SpanAllColumns)) {
        selected_symbol_ = row.symbol;
        if (symbol_selection_handler_) {
          symbol_selection_handler_(row.symbol);
        }
      }
      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("PREMIA_WATCHLIST_SYMBOL", row.symbol.c_str(),
                                  row.symbol.size() + 1);
        ImGui::Text("Move %s", row.symbol.c_str());
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget()) {
        if (const auto* payload =
                ImGui::AcceptDragDropPayload("PREMIA_WATCHLIST_SYMBOL")) {
          const auto dragged_symbol =
              std::string(static_cast<const char*>(payload->Data));
          MoveSymbolBefore(screen.watchlist.id, dragged_symbol, row.symbol);
        }
        ImGui::EndDragDropTarget();
      }
      ImGui::TableSetColumnIndex(2);
      ImGui::Text("%s", row.name.c_str());
      ImGui::TableSetColumnIndex(3);
      ImGui::Text("$%s", row.last_price.amount.c_str());
      ImGui::TableSetColumnIndex(4);
      ImGui::TextColored(DayChangeColor(row.day_change.absolute.amount),
                         "$%s (%s%%)", row.day_change.absolute.amount.c_str(),
                         row.day_change.percent.c_str());
      ImGui::TableSetColumnIndex(5);
      ImGui::Text("$%s", row.bid.amount.c_str());
      ImGui::TableSetColumnIndex(6);
      ImGui::Text("$%s", row.ask.amount.c_str());
      ImGui::TableSetColumnIndex(7);
      ImGui::Text("%s", SpreadText(row).c_str());
      ImGui::TableSetColumnIndex(8);
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

#include "watchlist_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "premia/core/application/composition_root.hpp"
#include "view/core/IconsMaterialDesign.h"

namespace premia {

namespace {

namespace pt = boost::property_tree;

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

auto WatchlistStatePath() -> std::filesystem::path {
  std::filesystem::path path;
  if (const char* override_path = std::getenv("PREMIA_RUNTIME_DIR");
      override_path != nullptr && override_path[0] != '\0') {
    path = std::filesystem::path(override_path);
#if defined(__APPLE__)
  } else if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    path = std::filesystem::path(home) / "Library/Application Support/Premia";
#elif defined(_WIN32)
  } else if (const char* appdata = std::getenv("APPDATA");
             appdata != nullptr && appdata[0] != '\0') {
    path = std::filesystem::path(appdata) / "Premia";
#else
  } else if (const char* xdg = std::getenv("XDG_STATE_HOME");
             xdg != nullptr && xdg[0] != '\0') {
    path = std::filesystem::path(xdg) / "premia";
  } else if (const char* home = std::getenv("HOME"); home != nullptr && home[0] != '\0') {
    path = std::filesystem::path(home) / ".local/state/premia";
#endif
  } else {
    path = std::filesystem::current_path() / ".premia-runtime";
  }
  std::error_code ec;
  std::filesystem::create_directories(path, ec);
  return path / "watchlist_state.json";
}

auto ContainsSymbol(const std::vector<std::string>& symbols, const std::string& symbol)
    -> bool {
  return std::find(symbols.begin(), symbols.end(), symbol) != symbols.end();
}

}  // namespace

void WatchlistView::LoadState() {
  if (state_loaded_) {
    return;
  }
  const auto state_path = WatchlistStatePath();
  if (!std::filesystem::exists(state_path)) {
    state_loaded_ = true;
    return;
  }

  try {
    pt::ptree tree;
    std::ifstream input(state_path);
    if (!input.good()) {
      state_loaded_ = true;
      return;
    }
    pt::read_json(input, tree);
    if (auto pinned = tree.get_child_optional("pinned")) {
      for (const auto& entry : *pinned) {
        std::vector<std::string> symbols;
        for (const auto& symbol : entry.second) {
          symbols.push_back(symbol.second.get_value<std::string>());
        }
        pinned_symbols_by_watchlist_[entry.first] = symbols;
      }
    }
    if (auto ordered = tree.get_child_optional("ordered")) {
      for (const auto& entry : *ordered) {
        std::vector<std::string> symbols;
        for (const auto& symbol : entry.second) {
          symbols.push_back(symbol.second.get_value<std::string>());
        }
        ordered_symbols_by_watchlist_[entry.first] = symbols;
      }
    }
  } catch (const std::exception&) {
  }
  state_loaded_ = true;
}

void WatchlistView::PersistState() const {
  pt::ptree tree;
  pt::ptree pinned_tree;
  for (const auto& [watchlist_id, symbols] : pinned_symbols_by_watchlist_) {
    pt::ptree symbol_tree;
    for (const auto& symbol : symbols) {
      pt::ptree value;
      value.put("", symbol);
      symbol_tree.push_back(std::make_pair("", value));
    }
    pinned_tree.add_child(watchlist_id, symbol_tree);
  }
  tree.add_child("pinned", pinned_tree);

  pt::ptree ordered_tree;
  for (const auto& [watchlist_id, symbols] : ordered_symbols_by_watchlist_) {
    pt::ptree symbol_tree;
    for (const auto& symbol : symbols) {
      pt::ptree value;
      value.put("", symbol);
      symbol_tree.push_back(std::make_pair("", value));
    }
    ordered_tree.add_child(watchlist_id, symbol_tree);
  }
  tree.add_child("ordered", ordered_tree);

  std::ofstream output(WatchlistStatePath());
  pt::write_json(output, tree);
}

void WatchlistView::EnsureWatchlistOrdering(
    const core::application::WatchlistScreenData& screen) {
  auto& ordered = ordered_symbols_by_watchlist_[screen.watchlist.id];
  auto& pinned = pinned_symbols_by_watchlist_[screen.watchlist.id];

  for (const auto& row : screen.rows) {
    if (!ContainsSymbol(ordered, row.symbol)) {
      ordered.push_back(row.symbol);
    }
  }
  ordered.erase(std::remove_if(ordered.begin(), ordered.end(),
                               [&screen](const std::string& symbol) {
                                 return std::none_of(screen.rows.begin(), screen.rows.end(),
                                                     [&symbol](const auto& row) {
                                                       return row.symbol == symbol;
                                                     });
                               }),
                ordered.end());

  pinned.erase(std::remove_if(pinned.begin(), pinned.end(),
                              [&screen](const std::string& symbol) {
                                return std::none_of(screen.rows.begin(), screen.rows.end(),
                                                    [&symbol](const auto& row) {
                                                      return row.symbol == symbol;
                                                    });
                              }),
               pinned.end());
}

auto WatchlistView::BuildOrderedRows(
    const core::application::WatchlistScreenData& screen) const
    -> std::vector<core::application::WatchlistRow> {
  std::vector<core::application::WatchlistRow> rows = screen.rows;
  const auto order_it = ordered_symbols_by_watchlist_.find(screen.watchlist.id);
  const auto pin_it = pinned_symbols_by_watchlist_.find(screen.watchlist.id);

  auto order_index = [&](const std::string& symbol) {
    if (order_it == ordered_symbols_by_watchlist_.end()) {
      return rows.size();
    }
    const auto& ordered = order_it->second;
    const auto it = std::find(ordered.begin(), ordered.end(), symbol);
    return it == ordered.end() ? ordered.size()
                               : static_cast<size_t>(std::distance(ordered.begin(), it));
  };
  auto pin_index = [&](const std::string& symbol) {
    if (pin_it == pinned_symbols_by_watchlist_.end()) {
      return rows.size();
    }
    const auto& pinned = pin_it->second;
    const auto it = std::find(pinned.begin(), pinned.end(), symbol);
    return it == pinned.end() ? pinned.size()
                              : static_cast<size_t>(std::distance(pinned.begin(), it));
  };

  std::stable_sort(rows.begin(), rows.end(), [&](const auto& lhs, const auto& rhs) {
    const bool lhs_pinned = IsPinned(screen.watchlist.id, lhs.symbol);
    const bool rhs_pinned = IsPinned(screen.watchlist.id, rhs.symbol);
    if (lhs_pinned != rhs_pinned) {
      return lhs_pinned > rhs_pinned;
    }
    if (lhs_pinned && rhs_pinned) {
      return pin_index(lhs.symbol) < pin_index(rhs.symbol);
    }
    return order_index(lhs.symbol) < order_index(rhs.symbol);
  });
  return rows;
}

auto WatchlistView::IsPinned(const std::string& watchlist_id,
                             const std::string& symbol) const -> bool {
  const auto it = pinned_symbols_by_watchlist_.find(watchlist_id);
  if (it == pinned_symbols_by_watchlist_.end()) {
    return false;
  }
  return ContainsSymbol(it->second, symbol);
}

void WatchlistView::TogglePinned(const std::string& watchlist_id,
                                 const std::string& symbol) {
  auto& pinned = pinned_symbols_by_watchlist_[watchlist_id];
  const auto it = std::find(pinned.begin(), pinned.end(), symbol);
  if (it == pinned.end()) {
    pinned.insert(pinned.begin(), symbol);
  } else {
    pinned.erase(it);
  }
  PersistState();
}

void WatchlistView::MoveSymbolBefore(const std::string& watchlist_id,
                                     const std::string& symbol,
                                     const std::string& before_symbol) {
  if (symbol == before_symbol) {
    return;
  }
  auto& ordered = ordered_symbols_by_watchlist_[watchlist_id];
  const auto from = std::find(ordered.begin(), ordered.end(), symbol);
  const auto to = std::find(ordered.begin(), ordered.end(), before_symbol);
  if (from == ordered.end() || to == ordered.end()) {
    return;
  }

  const auto value = *from;
  ordered.erase(from);
  const auto insert_at = std::find(ordered.begin(), ordered.end(), before_symbol);
  ordered.insert(insert_at, value);

  auto& pinned = pinned_symbols_by_watchlist_[watchlist_id];
  const auto pinned_from = std::find(pinned.begin(), pinned.end(), symbol);
  const auto pinned_to = std::find(pinned.begin(), pinned.end(), before_symbol);
  if (pinned_from != pinned.end() && pinned_to != pinned.end()) {
    const auto pinned_value = *pinned_from;
    pinned.erase(pinned_from);
    const auto pinned_insert = std::find(pinned.begin(), pinned.end(), before_symbol);
    pinned.insert(pinned_insert, pinned_value);
  }

  PersistState();
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

void WatchlistView::DrawCoreWatchlistPreview() {
  LoadState();
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
  if (rename_watchlist_name_.empty()) {
    rename_watchlist_name_ = screen.watchlist.name;
  }
  EnsureWatchlistOrdering(screen);
  const auto rows = BuildOrderedRows(screen);

  if (ImGui::BeginTable("WatchlistMutationBar", 4,
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
    ImGui::EndTable();
  }

  if (!status_message_.empty()) {
    ImGui::TextDisabled("%s", status_message_.c_str());
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
      const auto pin_label = IsPinned(screen.watchlist.id, row.symbol) ? "*" : ".";
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

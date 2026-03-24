#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"
#include "view/view.h"

namespace premia {
class WatchlistView : public View {
 private:
  int watchlistIndex = 0;
  std::string selected_symbol_;
  std::string filter_text_;
  std::string new_watchlist_name_;
  std::string rename_watchlist_name_;
  std::string new_symbol_input_;
  std::string status_message_;
  bool show_archived_ = false;
  int destination_watchlist_index_ = 0;
  int movement_filter_ = 0;
  bool state_loaded_ = false;
  std::unordered_map<std::string, std::vector<std::string>> pinned_symbols_by_watchlist_;
  std::unordered_map<std::string, std::vector<std::string>> ordered_symbols_by_watchlist_;
  std::function<void(const std::string&)> symbol_selection_handler_;
  EventMap events;
  Logger logger;

  void DrawCoreWatchlistPreview();
  void DrawWatchlistSummary(const core::application::WatchlistScreenData& screen);
  void LoadState();
  void PersistState() const;
  void EnsureWatchlistOrdering(const core::application::WatchlistScreenData& screen);
  auto BuildOrderedRows(const core::application::WatchlistScreenData& screen) const
      -> std::vector<core::application::WatchlistRow>;
  auto IsPinned(const std::string& watchlist_id, const std::string& symbol) const
      -> bool;
  void TogglePinned(const std::string& watchlist_id, const std::string& symbol);
  void MoveSymbolBefore(const std::string& watchlist_id,
                        const std::string& symbol,
                        const std::string& before_symbol);

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string &, const EventHandler&) override;
  void SetSelectedSymbol(const std::string& symbol);
  void SetSymbolSelectionHandler(
      const std::function<void(const std::string&)>& handler);
  void Update() override;

 private:
  ImGuiTableFlags watchlistFlags =
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
};
}  // namespace premia
#endif

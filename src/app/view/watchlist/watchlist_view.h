#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include <functional>
#include <string>

#include "premia/core/application/screen_models.hpp"
#include "view/view.h"

namespace premia {
class WatchlistView : public View {
 private:
  int watchlistIndex = 0;
  std::string selected_symbol_;
  std::string filter_text_;
  int movement_filter_ = 0;
  std::function<void(const std::string&)> symbol_selection_handler_;
  EventMap events;
  Logger logger;

  void DrawCoreWatchlistPreview();
  void DrawWatchlistSummary(const core::application::WatchlistScreenData& screen);

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

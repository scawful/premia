#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "view/view.h"

namespace premia {
class WatchlistView : public View {
 private:
  int watchlistIndex = 0;
  EventMap events;
  Logger logger;

  void DrawCoreWatchlistPreview();

 public:
  std::string getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(const std::string &, const EventHandler&) override;
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

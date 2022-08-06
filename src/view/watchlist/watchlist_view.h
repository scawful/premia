#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "model/core/watchlist_model.h"
#include "singletons/TDA.hpp"
#include "view/view.h"

namespace premia {
class WatchlistView : public View {
 private:
  int currentService = 0;
  int watchlistIndex = 0;
  bool serviceChanged = true;
  bool isLoggedIn = false;
  EventMap events;
  Logger logger;
  WatchlistModel model;

  void DrawWatchlistMenu();
  void DrawWatchlistTable();

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

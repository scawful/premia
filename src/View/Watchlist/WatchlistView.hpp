#ifndef WatchlistView_hpp
#define WatchlistView_hpp

#include "Interface/TDA.hpp"
#include "Model/Core/WatchlistModel.hpp"
#include "View/View.hpp"

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

  void drawWatchlistMenu();
  void drawWatchlistTable();

 public:
  String getName() override;
  void addLogger(const Logger& logger) override;
  void addEvent(CRString, const EventHandler&) override;
  void update() override;

 private:
  ImGuiTableFlags watchlistFlags =
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable |
      ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
      ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
};
}  // namespace premia
#endif

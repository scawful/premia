#ifndef ViewManager_hpp
#define ViewManager_hpp

#include "imgui/imgui.h"

#include "Account/AccountView.hpp"
#include "Chart/ChartView.hpp"
#include "Console/ConsoleView.hpp"
#include "Core/PrimaryView.hpp"
#include "Login/LoginView.hpp"
#include "Menu/MenuView.hpp"
#include "Options/OptionChain.hpp"
#include "View/Chart/Subview/LinePlotChart.hpp"
#include "Virtual/View.hpp"
#include "Watchlist/WatchlistView.hpp"

namespace premia {
class ViewManager {
 private:
  using ViewMap = std::unordered_map<String, std::shared_ptr<View>>;
  ImGuiTableFlags viewColumnFlags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersH |
      ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_SizingStretchSame;

  // Local Variables
  bool isLoggedIn = false;
  bool menuActive = true;
  bool fontLoaded = false;

  // Premia Context Utilities
  ViewMap views;
  EventMap events;
  Logger consoleLogger;

  // Views
  std::shared_ptr<View> leftColView = std::make_shared<WatchlistView>();
  std::shared_ptr<View> currentView;
  std::shared_ptr<View> rightColView = std::make_shared<AccountView>();

  std::shared_ptr<LoginView> loginView = std::make_shared<LoginView>();
  std::shared_ptr<MenuView> menuView = std::make_shared<MenuView>();
  std::shared_ptr<ConsoleView> consoleView = std::make_shared<ConsoleView>();

  void startGuiFrame() const;
  void setLeftColumnView(std::shared_ptr<View> view);
  void setRightColumnView(std::shared_ptr<View> view);

 public:
  ViewManager();

  void transferEvents() const;
  void setLoggedIn();

  void addEventHandler(CRString key, const EventHandler& event);
  void setCurrentView(std::shared_ptr<View> newView);
  void update() const;
};
}  // namespace premia

#endif

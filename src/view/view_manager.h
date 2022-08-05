#ifndef ViewManager_hpp
#define ViewManager_hpp

#include <imgui/imgui.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "Account/account_view.h"
#include "Chart/chart_view.h"
#include "Console/console_view.h"
#include "Core/primary_view.h"
#include "Login/login_view.h"
#include "Menu/menu_view.h"
#include "Metatypes.hpp"
#include "Options/OptionChain.hpp"
#include "View/view.h"
#include "View/Chart/Subview/LinePlotChart.hpp"
#include "Watchlist/watchlist_view.h"

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

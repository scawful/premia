#ifndef Workspace_hpp
#define Workspace_hpp

#include "view/account/account_view.h"
#include "view/chart/chart_view.h"
#include "view/chart/subview/LinePlotChart.hpp"
#include "view/console/console_view.h"
#include "view/core/primary_view.h"
#include "view/login/login_view.h"
#include "view/menu/menu_view.h"
#include "view/options/option_chain.h"
#include "view/view.h"
#include "view/watchlist/watchlist_view.h"
#include "watchlist/watchlist_view.h"

/**
 * @brief Customizable Workspaces for Charts, Watchlists, Portfolios, News,
 * Console, and more
 *
 * @details Create a class that substitutes for ViewManager but allows for more
 * customization Has no concrete main view, rather allows you to open an
 * indeterminate amount of views There should still be some root view, perhaps a
 * widget or toolbar that holds the main buttons and functionality of inserting
 * other views...
 *
 *          View classes are designed so that they do not initialize an ImGui
 * window when they are created The view window is what we drag around and press
 * the x button to close (but within the larger Premia SDL window) So the
 * general flow of this is the following:
 *
 *          ImGui::SetNextWindowSize(ImVec2(X,Y), ImGuiCond_Appearing);
 *          ImGui::Begin("<View-Name>", args...);
 *            view.update();
 *          ImGui::End();
 *
 *          And this code can be called after pressing a button, inputting a
 * console command, or a key combination...
 *
 */
namespace premia {

class Workspace {
 public:
  Workspace() = default;

  void Update();

 private:
  MenuView menu_view_;
  PrimaryView primary_view_;
};

}  // namespace premia

#endif
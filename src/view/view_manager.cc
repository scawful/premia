#include "ViewManager.hpp"

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
#include "View/Chart/Subview/LinePlotChart.hpp"
#include "View/view.h"
#include "Watchlist/watchlist_view.h"

namespace premia {

constexpr size_t LOGIN_WIDTH = 220;
constexpr size_t LOGIN_HEIGHT = 200;

void ViewManager::startGuiFrame() const {
  const ImGuiIO& io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
  if (!isLoggedIn) dimensions = ImVec2(LOGIN_WIDTH, LOGIN_HEIGHT);

  ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar;

  if (isLoggedIn) flags += ImGuiWindowFlags_MenuBar;

  static bool windowOpen = true;
  if (!ImGui::Begin("Premia", &windowOpen, flags)) {
    ImGui::End();
    return;
  }

  if (!windowOpen) events.at("quit")();
}

void ViewManager::setLeftColumnView(std::shared_ptr<View> view) {
  auto viewName = view->getName();
  if (!views.count(viewName)) {
    views[view->getName()] = view;
    leftColView = view;
    transferEvents();
  } else {
    leftColView = views[view->getName()];
  }
}

void ViewManager::setRightColumnView(std::shared_ptr<View> view) {
  auto viewName = view->getName();
  if (!views.count(viewName)) {
    views[view->getName()] = view;
    rightColView = view;
    transferEvents();
  } else {
    rightColView = views[view->getName()];
  }
}

ViewManager::ViewManager() {
  consoleLogger =
      std::bind(&ConsoleView::addLogStd, consoleView, std::placeholders::_1);
  leftColView->addLogger(consoleLogger);
  rightColView->addLogger(consoleLogger);
  consoleView->addLogger(
      consoleLogger);  // you'd be surprised, but this is necessary
  menuView->addEvent("consoleView",
                     [this]() -> void { consoleView->update(); });
  menuView->addEvent("optionChainLeftCol", [this]() -> void {
    setLeftColumnView(std::make_shared<OptionChainView>());
  });
  menuView->addEvent("optionChainRightCol", [this]() -> void {
    setRightColumnView(std::make_shared<OptionChainView>());
  });
  addEventHandler("goHome", [this]() -> void {
    setCurrentView(std::make_shared<PrimaryView>());
  });
  addEventHandler("chartView", [this]() -> void {
    setCurrentView(std::make_shared<ChartView>());
  });
  addEventHandler("linePlotView", [this]() -> void {
    setCurrentView(std::make_shared<LinePlotChart>());
  });
  addEventHandler("optionChainView", [this]() -> void {
    setCurrentView(std::make_shared<OptionChainView>());
  });
}

void ViewManager::transferEvents() const {
  for (const auto& [key, event] : events) {
    menuView->addEvent(key, event);
    currentView->addEvent(key, event);
    currentView->addLogger(consoleLogger);
  }
}

void ViewManager::setLoggedIn() { isLoggedIn = true; }

void ViewManager::addEventHandler(CRString key, const EventHandler& event) {
  events[key] = event;
  menuView->addEvent(key, event);
  if (!isLoggedIn) {
    loginView->addEvent(key, event);
  } else {
    currentView->addEvent(key, event);
    currentView->addLogger(consoleLogger);
  }
}

void ViewManager::setCurrentView(std::shared_ptr<View> newView) {
  auto viewName = newView->getName();
  if (!views.count(viewName)) {
    views[newView->getName()] = newView;
    currentView = newView;
  } else {
    currentView = views[newView->getName()];
  }
  transferEvents();
}

void ViewManager::update() const {
  startGuiFrame();
  if (menuActive) menuView->update();

  if (!isLoggedIn) {
    loginView->update();
  } else {
    if (ImGui::BeginTable("Main", 3, viewColumnFlags,
                          ImGui::GetContentRegionAvail())) {
      ImGui::TableSetupColumn(leftColView->getName().c_str());
      ImGui::TableSetupColumn(currentView->getName().c_str());
      ImGui::TableSetupColumn(rightColView->getName().c_str());

      ImGui::TableHeadersRow();
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::BeginChild("WatchlistRegion",
                        ImVec2(ImGui::GetContentRegionAvail().x, 0.f), false,
                        ImGuiWindowFlags_None);
      leftColView->update();
      ImGui::EndChild();

      ImGui::TableSetColumnIndex(1);
      currentView->update();

      ImGui::TableSetColumnIndex(2);
      rightColView->update();

      ImGui::EndTable();
    }
  }

  ImGui::End();
}

}  // namespace premia

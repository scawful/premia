#include "menu_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <string>

#include "metatypes.h"
#include "model/model.h"
#include "view/core/IconsMaterialDesign.h"
#include "view/view.h"

namespace premia {

namespace {

void DispatchEvent(const EventMap& events, const std::string& key) {
  const auto event = events.find(key);
  if (event != events.end()) {
    event->second();
  }
}

}  // namespace

void MenuView::DrawFileMenu() {
  static bool show_console = false;

  if (show_console) {
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Appearing);
    ImGui::Begin("Console", &show_console, ImGuiWindowFlags_NoScrollbar);
    DispatchEvent(events, "consoleView");
    ImGui::End();
  }

  if (ImGui::BeginMenu(ICON_MD_DASHBOARD)) {
    ImGui::MenuItem("New Workspace", "CTRL + N");
    ImGui::Separator();
    ImGui::MenuItem("Open Workspace", "CTRL + O");
    if (ImGui::BeginMenu("Open Recent")) {
      ImGui::MenuItem("None");
      ImGui::EndMenu();
    }
    ImGui::Separator();
    ImGui::MenuItem("Save Workspace", "CTRL + S");
    ImGui::Separator();
    ImGui::MenuItem("Open Console", ICON_MD_TERMINAL, &show_console);
    ImGui::Separator();
    if (ImGui::BeginMenu("Preferences")) {
      ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Quit", "ESC")) {
      DispatchEvent(events, "quit");
    }

    ImGui::EndMenu();
  }
}

void MenuView::DrawTradeMenu() {
  if (ImGui::BeginMenu(ICON_MD_SYNC_ALT)) {
    if (ImGui::MenuItem("Trade Desk")) {
      DispatchEvent(events, "tradeDeskView");
    }
    if (ImGui::MenuItem("Account Snapshot")) {
      DispatchEvent(events, "accountView");
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Option Chain")) {
      DispatchEvent(events, "optionChainView");
    }
    ImGui::EndMenu();
  }
}

void MenuView::DrawChartsMenu() {
  if (ImGui::BeginMenu(ICON_MD_ADD_CHART)) {
    if (ImGui::MenuItem("Chart Desk", ICON_MD_CANDLESTICK_CHART)) {
      DispatchEvent(events, "chartView");
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Line View", ICON_MD_SHOW_CHART)) {
      DispatchEvent(events, "linePlotView");
    }

    if (ImGui::MenuItem("Option Chain", ICON_MD_STACKED_LINE_CHART)) {
      DispatchEvent(events, "optionChainView");
    }

    ImGui::EndMenu();
  }
}

void MenuView::DrawAnalyzeMenu() {
  if (ImGui::BeginMenu(ICON_MD_TOPIC)) {
    if (ImGui::MenuItem("Overview")) DispatchEvent(events, "goHome");
    if (ImGui::MenuItem("Watchlists")) DispatchEvent(events, "watchlistView");
    if (ImGui::MenuItem("Account")) DispatchEvent(events, "accountView");
    ImGui::Separator();
    ImGui::TextDisabled("Research surfaces beyond options are still product backlog work.");
    ImGui::Separator();
    ImGui::EndMenu();
  }
}

void MenuView::DrawViewMenu() {
  static bool show_imgui_metrics = false;
  static bool show_implot_metrics = false;
  static bool show_imgui_style_editor = false;
  static bool show_implot_style_editor = false;
  if (show_imgui_metrics) {
    ImGui::ShowMetricsWindow(&show_imgui_metrics);
  }
  if (show_implot_metrics) {
    ImPlot::ShowMetricsWindow(&show_implot_metrics);
  }
  if (show_imgui_style_editor) {
    ImGui::Begin("Style Editor (ImGui)", &show_imgui_style_editor);
    ImGui::ShowStyleEditor();
    ImGui::End();
  }
  if (show_implot_style_editor) {
    ImGui::SetNextWindowSize(ImVec2(415, 762), ImGuiCond_Appearing);
    ImGui::Begin("Style Editor (ImPlot)", &show_implot_style_editor);
    ImPlot::ShowStyleEditor();
    ImGui::End();
  }

  if (ImGui::BeginMenu(ICON_MD_TUNE)) {
    if (ImGui::MenuItem("Overview")) DispatchEvent(events, "goHome");
    if (ImGui::MenuItem("Charts")) DispatchEvent(events, "chartView");
    if (ImGui::MenuItem("Trade Desk")) DispatchEvent(events, "tradeDeskView");
    if (ImGui::MenuItem("Account")) DispatchEvent(events, "accountView");
    ImGui::Separator();

    if (ImGui::BeginMenu("Appearance")) {
      if (ImGui::MenuItem("Fullscreen")) {
        DispatchEvent(events, "toggleFullscreenMode");
      }
      ImGui::EndMenu();
    }

    ImGui::Separator();
    if (ImGui::BeginMenu("GUI Tools")) {
      ImGui::MenuItem("Metrics (ImGui)", nullptr, &show_imgui_metrics);
      ImGui::MenuItem("Metrics (ImPlot)", nullptr, &show_implot_metrics);
      ImGui::MenuItem("Style Editor (ImGui)", nullptr,
                      &show_imgui_style_editor);
      ImGui::MenuItem("Style Editor (ImPlot)", nullptr,
                      &show_implot_style_editor);
      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
}

void MenuView::DrawHelpMenu() {
  if (ImGui::BeginMenu(ICON_MD_HELP)) {
    ImGui::MenuItem("Get Started");
    ImGui::MenuItem("Tips and Tricks");
    if (ImGui::MenuItem("About")) about = true;

    ImGui::EndMenu();
  }
}

void MenuView::DrawScreen() {
  if (ImGui::BeginMenuBar()) {
    DrawFileMenu();
    DrawTradeMenu();
    DrawChartsMenu();
    DrawAnalyzeMenu();
    DrawViewMenu();
    DrawHelpMenu();
    ImGui::EndMenuBar();
  }

  if (about) ImGui::OpenPopup("About");

  if (ImGui::BeginPopupModal("About", nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Premia Version 0.4");
    ImGui::Text("Written by: Justin Scofield (scawful)");
    ImGui::Text("Dependencies: Boost, SDL2, ImGui, ImPlot, Abseil");
    ImGui::Text("Data providers: Schwab, IBKR, Plaid, local preview");

    if (ImGui::Button("Close", ImVec2(120, 0))) {
      about = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

std::string MenuView::getName() { return "Menu"; }

void MenuView::addLogger(const Logger& newLogger) { this->logger = newLogger; }

void MenuView::addEvent(const std::string& key, const EventHandler& event) {
  this->events[key] = event;
}

void MenuView::Update() { DrawScreen(); }
}  // namespace premia

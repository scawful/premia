#include "workspace.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <memory>
#include <string>

#include "metatypes.h"
// #include "core/HLXT.hpp"
#include "view/chart/subview/candle_chart.h"
#include "view/view.h"

namespace premia {

namespace {

constexpr ImGuiWindowFlags kMainEditorFlags =
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar;

void NewMasterFrame() {
  const ImGuiIO& io = ImGui::GetIO();
  ImGui::NewFrame();
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
  ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

  if (!ImGui::Begin("##PremiaWorkspace", nullptr, kMainEditorFlags)) {
    ImGui::End();
    return;
  }
}

}  // namespace

void Workspace::Update() {
  NewMasterFrame();
  menu_view_.Update();
  primary_view_.Update();
  ImGui::End();
}

}  // namespace premia
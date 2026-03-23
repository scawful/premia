#include "login_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "premia/core/application/composition_root.hpp"
#include "view/core/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {

void LoginView::DrawScreen() const {
  static bool tdaAuth = false;
  static bool schwabAuth = true;
  static std::string username;
  static std::string password;
  static std::string statusMessage;

  if (ImGui::BeginTable("split", 2, ImGuiTableFlags_None)) {
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_PERSON);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(175.f);
    ImGui::InputText("##username", &username);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_PASSWORD);
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(175.f);
    ImGui::InputText("##password", &password);

    ImGui::EndTable();
  }

  ImGui::Separator();
  ImGui::Text("Primary Brokerage");
  ImGui::TextColored(ImVec4(0.40f, 0.72f, 0.96f, 1.0f), "Charles Schwab");
  ImGui::TextDisabled(
      "Use the Schwab workflow below. TDA is preserved only as a transitional path.");

  if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvail().x, 20.f))) {
    bool shouldEnterWorkspace = true;

    if (tdaAuth) {
      statusMessage =
          "TDA now uses provider-backed runtime config storage; no direct login call is needed.";
      if (logger) logger(std::string(statusMessage));
    }
    if (schwabAuth) {
      auto& workflows = core::application::CompositionRoot::Instance().Workflows();
      if (!password.empty()) {
        const auto result = workflows.CompleteSchwabOAuth({password, username});
        statusMessage = "Schwab workflow state: " + result.display_name + " -> " +
                        core::domain::ConnectionStatusToString(result.status);
      } else {
        const auto launch = workflows.StartSchwabOAuth({username, "desktop"});
        statusMessage = "Open this Schwab auth URL in a browser: " + launch.auth_url;
        shouldEnterWorkspace = false;
      }
      if (logger) logger(std::string(statusMessage));
    }

    if (shouldEnterWorkspace && events.count("login")) {
      events.at("login")();
    }
  }

  ImGui::Separator();
  if (ImGui::BeginTable("APIICON", 3, ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_API);
    ImGui::TableNextColumn();
    ImGui::Checkbox("TDA (Legacy)", &tdaAuth);
    ImGui::TableNextColumn();
    ImGui::Checkbox("Schwab (Primary)", &schwabAuth);
    ImGui::EndTable();
  }
  if (!statusMessage.empty()) {
    ImGui::Separator();
    ImGui::TextWrapped("%s", statusMessage.c_str());
  }
}

std::string LoginView::getName() { return "Login"; }

void LoginView::addLogger(const Logger &newLogger) { this->logger = newLogger; }

void LoginView::addEvent(const std::string &key, const EventHandler &event) {
  this->events[key] = event;
}

void LoginView::Update() { DrawScreen(); }
}  // namespace premia

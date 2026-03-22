#include "login_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "core/Schwab.hpp"
#include "core/TDA.hpp"
#include "view/core/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {

void LoginView::DrawScreen() const {
  static bool tdaAuth;
  static bool schwabAuth;
  static std::string username;
  static std::string password;

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

  if (ImGui::Button("Login", ImVec2(ImGui::GetContentRegionAvail().x, 20.f))) {
    if (tdaAuth) {
      std::ifstream keyfile("assets/apikey.txt");
      std::string consumer_key = username;
      std::string refresh_token = password;
      if (keyfile.good()) {
        try {
          std::stringstream buffer;
          buffer << keyfile.rdbuf();
          buffer >> consumer_key;
          buffer >> refresh_token;
          keyfile.close();
        } catch (int e) {
          // If issue reading key file, use the keys in the fields
          consumer_key = username;
          refresh_token = password;
        }
      }
      tda::TDA::getInstance().authUser(consumer_key, refresh_token);
    }
    if (schwabAuth) {
      auto& schwab = schwab::Schwab::getInstance();
      if (schwab.loadConfig()) {
        // Try loading persisted tokens first.
        schwab.loadTokens();
        if (!schwab.isAuthenticated() && schwab.hasRefreshToken()) {
          schwab.refreshAuth();
          schwab.saveTokens();
        }
        if (!schwab.isAuthenticated()) {
          if (!password.empty()) {
            // Treat password field as the callback URL / auth code.
            if (schwab.exchangeAuthCode(password)) {
              schwab.saveTokens();
            }
          } else {
            // No tokens and no code — open browser for first-time auth.
            schwab.openAuthBrowser();
          }
        }
        if (schwab.isAuthenticated()) {
          schwab.bootstrapAccounts();
        }
      }
    }
    events.at("login")();
  }

  ImGui::Separator();
  if (ImGui::BeginTable("APIICON", 3, ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_API);
    ImGui::TableNextColumn();
    ImGui::Checkbox("TDA", &tdaAuth);
    ImGui::TableNextColumn();
    ImGui::Checkbox("Schwab", &schwabAuth);
    ImGui::EndTable();
  }
}

std::string LoginView::getName() { return "Login"; }

void LoginView::addLogger(const Logger &newLogger) { this->logger = newLogger; }

void LoginView::addEvent(const std::string &key, const EventHandler &event) {
  this->events[key] = event;
}

void LoginView::Update() { DrawScreen(); }
}  // namespace premia

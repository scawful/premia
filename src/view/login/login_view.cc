#include "login_view.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <string>

#include "singletons/TDA.hpp"
#include "gfx/IconsMaterialDesign.h"
#include "metatypes.h"
#include "model/model.h"
#include "view/view.h"

namespace premia {

static const char *_IMGUIGetClipboardText(void *) {
  return SDL_GetClipboardText();
}
static void _IMGUISetClipboardText(void *, const char *text) {
  SDL_SetClipboardText(text);
}

void LoginView::DrawScreen() const {
  ImGuiIO &io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
  io.KeyMap[ImGuiKey_Enter] = SDL_GetScancodeFromKey(SDLK_RETURN);
  io.KeyMap[ImGuiKey_UpArrow] = SDL_GetScancodeFromKey(SDLK_UP);
  io.KeyMap[ImGuiKey_DownArrow] = SDL_GetScancodeFromKey(SDLK_DOWN);
  io.KeyMap[ImGuiKey_Tab] = SDL_GetScancodeFromKey(SDLK_TAB);
  io.KeyMap[ImGuiKey_ModCtrl] = SDL_GetScancodeFromKey(SDLK_LCTRL);
  static bool tdaAuth;
  static bool coinbaseAuth;
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
    events.at("login")();
  }

  ImGui::Separator();
  if (ImGui::BeginTable("APIICON", 3, ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::Text(ICON_MD_API);
    ImGui::TableNextColumn();
    ImGui::Checkbox("TDA", &tdaAuth);
    ImGui::TableNextColumn();
    ImGui::Checkbox("CB", &coinbaseAuth);
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

#include "LoginView.hpp"
#include "Interface/TDA.hpp"

static const char* _IMGUIGetClipboardText(void *) { return SDL_GetClipboardText(); }
static void _IMGUISetClipboardText(void *, const char *text) { SDL_SetClipboardText(text); }

void LoginView::drawScreen() const
{
    ImGuiIO & io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Backspace] = SDL_GetScancodeFromKey(SDLK_BACKSPACE);
    io.KeyMap[ImGuiKey_Enter] = SDL_GetScancodeFromKey(SDLK_RETURN);
    io.KeyMap[ImGuiKey_UpArrow] = SDL_GetScancodeFromKey(SDLK_UP);
    io.KeyMap[ImGuiKey_DownArrow] = SDL_GetScancodeFromKey(SDLK_DOWN);
    io.KeyMap[ImGuiKey_Tab] = SDL_GetScancodeFromKey(SDLK_TAB);
    io.KeyMap[ImGuiKey_ModCtrl] = SDL_GetScancodeFromKey(SDLK_LCTRL);
    static bool tdaAuth;
    static bool coinbaseAuth;
    static String username;
    static String password;
    
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

    if (ImGui::Button("Login", 
        ImVec2(ImGui::GetContentRegionAvail().x, 20.f)) ) {
        if (tdaAuth) {
            std::ifstream keyfile("assets/apikey.txt");
            String consumer_key;
            String refresh_token;
            if (keyfile.good()) {
                std::stringstream buffer;
                buffer << keyfile.rdbuf();
                keyfile.close();
                buffer >> consumer_key;
                buffer >> refresh_token;
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

String 
LoginView::getName() {
    return "Login";
}

void LoginView::addLogger(const Logger& newLogger)
{
    this->logger = newLogger;
}

void LoginView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void LoginView::update() 
{
    drawScreen();
}

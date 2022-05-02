#include "LoginView.hpp"
#include "TDA.hpp"

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
    static char username[128];
    static char password[128];
    
    static String consumer_key;
    static String refresh_token;
    
    String welcomePrompt = "Welcome to Premia"; 
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(welcomePrompt.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", welcomePrompt.c_str());

    io.SetClipboardTextFn = _IMGUISetClipboardText; 
    io.GetClipboardTextFn = _IMGUIGetClipboardText;

    ImGui::Text("Consumer Key: "); ImGui::SameLine(); ImGui::InputText("##username", &consumer_key);
    ImGui::Text("Refresh Token: "); ImGui::SameLine(); ImGui::InputText("##password", &refresh_token);
    
    if ( ImGui::Button("Login") ) {
        std::ifstream keyfile("assets/apikey.txt");
        if (keyfile.good()) {
            std::stringstream buffer;
            buffer << keyfile.rdbuf();
            keyfile.close();
            buffer >> consumer_key;
            buffer >> refresh_token;
        } else {
            std::cout << "fail!" << std::endl;
        }
        tda::TDA::getInstance().authUser(consumer_key, refresh_token);
        events.at("login")();
    }
}

void LoginView::addLogger(const ConsoleLogger & newLogger)
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

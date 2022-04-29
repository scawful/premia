#include "LoginView.hpp"

void LoginView::drawScreen() const
{
    static char username[128];
    static char password[128];
    
    std::string welcomePrompt = "Welcome to Premia"; 
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(welcomePrompt.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", welcomePrompt.c_str());

    ImGui::Text("Username: "); ImGui::SameLine(); ImGui::InputText("##username", username, IM_ARRAYSIZE(username));
    ImGui::Text("Password: "); ImGui::SameLine(); ImGui::InputText("##password", password, IM_ARRAYSIZE(password));
    
    if ( ImGui::Button("Login") ) {
        events.at("login")();
    }
}

void LoginView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void LoginView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void LoginView::update() 
{
    drawScreen();
}

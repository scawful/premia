#include "LoginView.hpp"

void LoginView::addEvent(const VoidEventHandler & event) 
{
    this->loginEvent = event;
}

void LoginView::update() 
{
    static char username[128] = "";
    static char password[128] = "";

    std::string welcomePrompt = "Welcome to Premia"; 
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(welcomePrompt.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", welcomePrompt.c_str());

    ImGui::Text("Username: "); ImGui::SameLine(); ImGui::InputText("##username",username, IM_ARRAYSIZE(username));
    ImGui::Text("Password: "); ImGui::SameLine(); ImGui::InputText("##password",password, IM_ARRAYSIZE(password));
    
    if ( ImGui::Button("Login") ) {
        loginEvent();
    }
}

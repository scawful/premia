#include "LoginView.hpp"

void LoginView::drawScreen() const
{
    ImGui::StyleColorsClassic();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300,200), ImGuiCond_Always);

    if (!ImGui::Begin("Premia Login", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

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

    ImGui::End();
}

void LoginView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void LoginView::update() 
{
    drawScreen();
}

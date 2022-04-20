#include "LoginView.hpp"

void LoginView::drawScreen() const
{
    ImGui::StyleColorsClassic();
    ImGui::NewFrame();
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300,400), ImGuiCond_Always);

    if (!ImGui::Begin("Premia Login", nullptr, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

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

    ImGui::End();
}

void LoginView::addEvent(const VoidEventHandler & event) 
{
    this->loginEvent = event;
}

void LoginView::update() 
{
    drawScreen();
}

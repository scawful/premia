#include "ViewManager.hpp"

ViewManager::ViewManager()
{
    this->events["toggleFixedDimensions"] = [this] () -> void {
        if (this->fixedDimensions) {
            this->fixedDimensions = false;
        } else {
            this->fixedDimensions = true;
        }
    };
}

void
ViewManager::transferEvents()
{
    for (const auto & [key, event] : this->events) {
        this->menuView->addEvent(key, event);
        this->currentView->addEvent(key, event);
    }
}

void 
ViewManager::setLoggedIn()
{
    this->isLoggedIn = true;
}

void
ViewManager::setConsoleView()
{
    if (this->consoleActive) {
        this->consoleActive = false;
    } else {
        this->consoleActive = true;
    }
}

void 
ViewManager::startGuiFrame() const
{
    ImGui::StyleColorsClassic();
    ImGui::NewFrame();  
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
    if (this->isLoggedIn) {
        if (this->consoleActive) {
            dimensions.y = io.DisplaySize.y * (float) 0.70;
        }
    } else {
        ImGui::SetNextWindowSize(ImVec2(300,200), ImGuiCond_Always);
    }
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
    if (!ImGui::Begin("Premia", nullptr, flags)) {
        ImGui::End();
        return;
    }
}

void 
ViewManager::endGuiFrame() const 
{
    ImGui::End();
}

void
ViewManager::displayConsole() const
{
    ImGui::End();
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y * 0.70f));
    auto size = ImVec2(io.DisplaySize.x, io.DisplaySize.y * 0.30f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    this->consoleView->update();
}

void 
ViewManager::addEventHandler(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
    this->menuView->addEvent(key, event);
    this->currentView->addEvent(key, event);
}

void 
ViewManager::setCurrentView(std::shared_ptr<View> newView)
{
    this->currentView = newView;
}

void 
ViewManager::updateCurrentView() const
{
    this->startGuiFrame();

    if (menuActive)
        this->menuView->update();
    
    this->currentView->update();

    if (consoleActive)
        this->displayConsole();
    
    this->endGuiFrame();
}
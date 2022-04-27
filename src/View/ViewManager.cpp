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
ViewManager::startGuiFrame() const
{
    ImGui::StyleColorsClassic();
    ImGui::NewFrame();  
    ImGui::SetNextWindowPos( ImVec2(0, 0) );
    const ImGuiIO & io = ImGui::GetIO();

    if (isLoggedIn) {
        if (fixedDimensions) {
            ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);
            ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);
        }
    } else {
        ImGui::SetNextWindowSize(ImVec2(300,200), ImGuiCond_Always);
    }

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
    this->endGuiFrame();
}
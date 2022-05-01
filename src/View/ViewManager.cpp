#include "ViewManager.hpp"

void ViewManager::shareEvents()
{

}

ViewManager::ViewManager()
{
    this->consoleLogger = std::bind(&ConsoleView::addLogStd, consoleView, std::placeholders::_1);
    this->watchlistView->addLogger(this->consoleLogger);
}

void
ViewManager::transferEvents() const
{
    for (const auto & [key, event] : this->events) {
        this->menuView->addEvent(key, event);
        this->currentView->addEvent(key, event);
    }
}

void 
ViewManager::setLoggedIn(const std::string & key, const std::string & token)
{
    this->currentView->addAuth(key, token);
    this->watchlistView->addAuth(key, token);
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
ViewManager::setWatchlistView()
{
    if (this->watchlistActive) {
        this->watchlistActive = false;
    } else {
        this->watchlistActive = true;
    }
}

void 
ViewManager::startGuiFrame() const
{    
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::NewFrame();  
    ImGui::SetNextWindowPos( ImVec2(0, 0) );

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);

    if (!this->isLoggedIn)
        dimensions = ImVec2(300,200);

    if (this->consoleActive)
        dimensions.y = io.DisplaySize.y * 0.75f;

    if (this->watchlistActive)
        dimensions.x = io.DisplaySize.x * 0.75f;

    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoCollapse | 
                             ImGuiWindowFlags_NoBringToFrontOnFocus | 
                             ImGuiWindowFlags_NoScrollbar;

    if (isLoggedIn) {
        flags += ImGuiWindowFlags_MenuBar;
    }
                               
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
    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y * 0.75f));
    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y * 0.25f);
    if (watchlistActive)
        dimensions.x = io.DisplaySize.x * 0.75f;
        
    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);
    this->consoleView->update();
}

void 
ViewManager::displayWatchlist() const
{
    ImGui::End();    
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.75f, 0));
    auto size = ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    this->watchlistView->update();
}

void 
ViewManager::addEventHandler(const std::string & key, const Premia::EventHandler & event)
{
    this->events[key] = event;
    this->menuView->addEvent(key, event);
    if (!isLoggedIn) {
        this->loginView->addEvent(key, event);
    } else {
        this->currentView->addEvent(key, event);
        this->currentView->addLogger(consoleLogger);
    }
}

// template <typename T>
// void ViewManager::addEvent(const std::string & key, T event)
// {
//     this->allEvents.insert<T>(key, event);
// }

void ViewManager::addLoginEvent(const Premia::TDALoginEvent & event)
{
    this->loginView->addLoginEvent(event);
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
    
    if (!isLoggedIn)
        this->loginView->update();
    else
        this->currentView->update();

    if (watchlistActive)
        this->displayWatchlist();
    
    if (consoleActive)
        this->displayConsole();
    
    this->endGuiFrame();
}
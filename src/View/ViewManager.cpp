#include "ViewManager.hpp"

void 
ViewManager::startGuiFrame() const
{    
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::NewFrame();  
    ImGui::SetNextWindowPos( ImVec2(0, 0) );

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);

    if (!this->isLoggedIn)
        dimensions = ImVec2(400,250);

    ImGui::SetNextWindowSize(dimensions, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoCollapse | 
                             ImGuiWindowFlags_NoBringToFrontOnFocus | 
                             ImGuiWindowFlags_NoScrollbar;

    if (isLoggedIn) {
        flags += ImGuiWindowFlags_MenuBar;
    }
    
    static bool windowOpen = true;
    if (!ImGui::Begin("Premia", &windowOpen, flags)) {
        ImGui::End();
        return;
    }

    if (!windowOpen) {
        events.at("quit")();
    }
}

ViewManager::ViewManager() {
    this->consoleLogger = std::bind(&ConsoleView::addLogStd, 
                                    consoleView, std::placeholders::_1);
    this->watchlistView->addLogger(this->consoleLogger);
    this->accountView->addLogger(this->consoleLogger);
}

void
ViewManager::transferEvents() const {
    for (const auto & [key, event] : this->events) {
        this->menuView->addEvent(key, event);
        this->currentView->addEvent(key, event);
    }
}

void 
ViewManager::setLoggedIn() {
    this->isLoggedIn = true;
}


void 
ViewManager::addEventHandler(CRString key, const EventHandler & event) {
    this->events[key] = event;
    this->menuView->addEvent(key, event);
    if (!isLoggedIn) {
        this->loginView->addEvent(key, event);
    } else {
        this->currentView->addEvent(key, event);
        this->currentView->addLogger(consoleLogger);
    }
}

void 
ViewManager::setCurrentView(std::shared_ptr<View> newView) {
    this->currentView = newView;
}

void 
ViewManager::update() const
{
    this->startGuiFrame();
    if (menuActive)
        this->menuView->update();

    if (!isLoggedIn) {
        this->loginView->update();
    } else {
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | 
                                        ImGuiTableFlags_BordersH | 
                                        ImGuiTableFlags_BordersV |
                                        ImGuiTableFlags_Hideable |
                                        ImGuiTableFlags_Reorderable |
                                        ImGuiTableFlags_SizingStretchSame;

        if (ImGui::BeginTable("table1", 3, flags, ImGui::GetContentRegionAvail())) {            
            ImGui::TableSetupColumn(this->watchlistView->getName().c_str());
            ImGui::TableSetupColumn(this->currentView->getName().c_str());
            ImGui::TableSetupColumn(this->accountView->getName().c_str());
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); 
            ImGui::BeginChild("WatchlistRegion", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), false, ImGuiWindowFlags_None);
            this->watchlistView->update(); 
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1); 
            this->currentView->update();
            this->consoleView->update();

            ImGui::TableSetColumnIndex(2); 
            this->accountView->update();

            ImGui::EndTable();
        }
    }
    
    ImGui::End();
}
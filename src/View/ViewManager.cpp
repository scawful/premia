#include "ViewManager.hpp"

void 
ViewManager::startGuiFrame() const
{    
    const ImGuiIO & io = ImGui::GetIO();
    ImGui::NewFrame();  
    ImGui::SetNextWindowPos( ImVec2(0, 0) );

    ImVec2 dimensions(io.DisplaySize.x, io.DisplaySize.y);

    if (!isLoggedIn)
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
    consoleLogger = std::bind(&ConsoleView::addLogStd, 
                                    consoleView, std::placeholders::_1);
    watchlistView->addLogger(consoleLogger);
    accountView->addLogger(consoleLogger);
    menuView->addEvent("consoleView", [this] () -> void {
        consoleView->update();
    });
    rightColView = accountView;
    menuView->addEvent("optionChainRightCol", [this] () -> void {
        rightColView = std::make_shared<OptionChainView>();
    });
}

void
ViewManager::transferEvents() const {
    for (const auto & [key, event] : events) {
        menuView->addEvent(key, event);
        currentView->addEvent(key, event);
        currentView->addLogger(consoleLogger);
    }
}

void 
ViewManager::setLoggedIn() {
    isLoggedIn = true;
}


void 
ViewManager::addEventHandler(CRString key, const EventHandler & event) {
    events[key] = event;
    menuView->addEvent(key, event);
    if (!isLoggedIn) {
        loginView->addEvent(key, event);
    } else {
        currentView->addEvent(key, event);
        currentView->addLogger(consoleLogger);
    }
}

void 
ViewManager::setCurrentView(std::shared_ptr<View> newView) {
    auto viewName = newView->getName();
    if (!viewMap.count(viewName)) {
        viewMap[newView->getName()] = newView;
        currentView = newView;
    } else {
        currentView = viewMap[newView->getName()];
    }
    transferEvents();
}

void 
ViewManager::update() const
{
    startGuiFrame();
    if (menuActive)
        menuView->update();

    if (!isLoggedIn) {
        loginView->update();
    } else {
        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | 
                                        ImGuiTableFlags_BordersH | 
                                        ImGuiTableFlags_BordersV |
                                        ImGuiTableFlags_Hideable |
                                        ImGuiTableFlags_Reorderable |
                                        ImGuiTableFlags_SizingStretchSame;

        if (ImGui::BeginTable("table1", 3, flags, ImGui::GetContentRegionAvail())) {            
            ImGui::TableSetupColumn(watchlistView->getName().c_str());
            ImGui::TableSetupColumn(currentView->getName().c_str());
            ImGui::TableSetupColumn(rightColView->getName().c_str());
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); 
            ImGui::BeginChild("WatchlistRegion", ImVec2(ImGui::GetContentRegionAvail().x, 0.f), false, ImGuiWindowFlags_None);
            watchlistView->update(); 
            ImGui::EndChild();

            ImGui::TableSetColumnIndex(1); 
            currentView->update();

            ImGui::TableSetColumnIndex(2); 
            rightColView->update();

            ImGui::EndTable();
        }
    }
    
    ImGui::End();
}
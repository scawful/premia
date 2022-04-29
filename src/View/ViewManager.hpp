#ifndef ViewManager_hpp
#define ViewManager_hpp

#include "core.hpp"
#include "View.hpp"
#include "PrimaryView.hpp"
#include "Core/LoginView.hpp"
#include "Core/MenuView.hpp"
#include "Core/ConsoleView.hpp"
#include "Core/WatchlistView.hpp"
#include "Dev/DebugView.hpp"

class ViewManager
{
private:
    bool isLoggedIn = false;
    bool menuActive = true;
    bool consoleActive = false;
    bool watchlistActive = false;
    bool fixedDimensions = true;
    std::shared_ptr<View> currentView = std::make_shared<LoginView>();
    std::shared_ptr<View> consoleView = std::make_shared<ConsoleView>();
    std::shared_ptr<View> watchlistView = std::make_shared<WatchlistView>();
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    std::unordered_map<std::string, VoidEventHandler> events;

public:
    ViewManager();

    void transferEvents();
    void setLoggedIn();
    void setConsoleView();
    void setWatchlistView();
    void startGuiFrame() const;
    void endGuiFrame() const;
    void displayConsole() const;
    void displayWatchlist() const;
    void addEventHandler(const std::string & key, const VoidEventHandler & event);
    void setCurrentView(std::shared_ptr<View> newView);
    void updateCurrentView() const;
};

#endif
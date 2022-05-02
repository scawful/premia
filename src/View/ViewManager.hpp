#ifndef ViewManager_hpp
#define ViewManager_hpp

#include "View.hpp"
#include "PrimaryView.hpp"
#include "Core/LoginView.hpp"
#include "Core/MenuView.hpp"
#include "Core/ConsoleView.hpp"
#include "Core/WatchlistView.hpp"

class ViewManager
{
private:
    bool isLoggedIn = false;
    bool menuActive = true;
    bool consoleActive = false;
    bool watchlistActive = false;
    bool fontLoaded = false;
    Premia::TDALoginEvent loginEvent;
    Premia::ConsoleLogger consoleLogger;
    std::shared_ptr<View> currentView;
    std::shared_ptr<LoginView> loginView = std::make_shared<LoginView>();
    std::shared_ptr<WatchlistView> watchlistView = std::make_shared<WatchlistView>();
    std::shared_ptr<MenuView> menuView = std::make_shared<MenuView>();
    std::shared_ptr<ConsoleView> consoleView = std::make_shared<ConsoleView>();
    std::unordered_map<String, Premia::EventHandler> events;
    Premia::EventInterface allEvents;

    void shareEvents();

public:
    ViewManager();

    void transferEvents() const;
    void setLoggedIn();
    void setLoggedIn(CRString, CRString);
    void setConsoleView();
    void setWatchlistView();
    void startGuiFrame() const;
    void endGuiFrame() const;
    void displayConsole() const;
    void displayWatchlist() const;
    void addEventHandler(CRString key, const Premia::EventHandler & event);

    // template <typename T>
    // void addEvent(CRString key, T event);

    void addLoginEvent(const Premia::TDALoginEvent &);
    void setCurrentView(std::shared_ptr<View> newView);
    void updateCurrentView() const;
};

#endif

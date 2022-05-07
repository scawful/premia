#ifndef ViewManager_hpp
#define ViewManager_hpp

#include "Virtual/View.hpp"
#include "Core/PrimaryView.hpp"
#include "Login/LoginView.hpp"
#include "Menu/MenuView.hpp"
#include "Core/ConsoleView.hpp"
#include "Watchlist/WatchlistView.hpp"
#include "Chart/ChartView.hpp"
#include "Options/OptionChain.hpp"
#include "Account/AccountView.hpp"

class ViewManager {
    using ViewMap = std::unordered_map<String, std::shared_ptr<View>>;
private:
    // Local Variables 
    bool isLoggedIn = false;
    bool menuActive = true;
    bool fontLoaded = false;
    
    // Premia Context Utilities 
    EventMap events;
    ConsoleLogger consoleLogger;

    // Views 
    std::shared_ptr<View> currentView;
    std::shared_ptr<View> rightColView;

    ViewMap viewMap;

    std::shared_ptr<LoginView> loginView = std::make_shared<LoginView>();
    std::shared_ptr<AccountView> accountView = std::make_shared<AccountView>();
    std::shared_ptr<WatchlistView> watchlistView = std::make_shared<WatchlistView>();
    std::shared_ptr<MenuView> menuView = std::make_shared<MenuView>();
    std::shared_ptr<ConsoleView> consoleView = std::make_shared<ConsoleView>();

    void startGuiFrame() const;

public:
    ViewManager();

    void transferEvents() const;
    void setLoggedIn();

    void addEventHandler(CRString key, const EventHandler & event);
    void setCurrentView(std::shared_ptr<View> newView);
    void update() const;
};

#endif

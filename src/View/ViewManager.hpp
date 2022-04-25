#ifndef ViewManager_hpp
#define ViewManager_hpp

#include "core.hpp"
#include "View.hpp"
#include "PrimaryView.hpp"
#include "Core/LoginView.hpp"
#include "Core/MenuView.hpp"
#include "Dev/DebugView.hpp"

class ViewManager
{
private:
    std::shared_ptr<View> currentView = std::make_shared<LoginView>();
    std::unordered_map<std::string, VoidEventHandler> events;

public:
    ViewManager()=default;
    explicit ViewManager(const VoidEventHandler & event);

    void transferEvents();
    void addEventHandler(const std::string & key, const VoidEventHandler & event);
    void setCurrentView(std::shared_ptr<View> newView);
    void updateCurrentView() const;
};

#endif
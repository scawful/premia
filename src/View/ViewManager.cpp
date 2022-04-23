#include "ViewManager.hpp"

ViewManager::ViewManager(const VoidEventHandler & event)
{
    addEventHandler("initEvent", event);
}

void 
ViewManager::addEventHandler(const std::string & key, const VoidEventHandler & event) const
{
    this->events[key] = event;
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
    this->currentView->update();
}
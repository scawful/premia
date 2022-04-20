#include "ViewManager.hpp"

ViewManager::ViewManager(const DebugView & newView, const VoidEventHandler & event)
{
    this->currentView = std::make_shared<DebugView>(newView);
    addEventHandler(event);
}

void 
ViewManager::addEventHandler(const VoidEventHandler & event) const
{
    this->currentView->addEvent(event);
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
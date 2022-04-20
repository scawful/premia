#include "ViewManager.hpp"

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
#include "PrimaryController.hpp"

void 
PrimaryController::initCallbacks()
{
    loginCallback = [this] () -> void { 
        viewManager.setCurrentView(std::make_shared<PrimaryView>()); 
    };

    viewManager.addEventHandler(loginCallback);
}

PrimaryController::PrimaryController(const Model & nm, const ViewManager & vm) 
    : model(nm), viewManager(vm) { this->initCallbacks(); }

void 
PrimaryController::onLoad()
{
    this->viewManager.updateCurrentView();
}
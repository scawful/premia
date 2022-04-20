#ifndef PrimaryController_hpp
#define PrimaryController_hpp

#include "Controller.hpp"
#include "../Model/PrimaryModel.hpp"
#include "../View/ViewManager.hpp"

class PrimaryController : public Controller
{
private:
    Model model;
    ViewManager viewManager;
    VoidEventHandler loginCallback;

    void initCallbacks();

public:
    PrimaryController(const Model & nm, const ViewManager & vm);

    void onLoad();
};

#endif
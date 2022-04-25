#ifndef Controller_hpp
#define Controller_hpp

#include "../../include/core.hpp"
#include "../Model/Model.hpp"
#include "../View/ViewManager.hpp"
#include "../View/Core/LoginView.hpp"
#include "../View/Dev/DebugView.hpp"
#include "../View/Chart/ChartView.hpp"

class Controller
{
public:
    Controller()=default;
    virtual ~Controller()=default;

    virtual void onEntry()  = 0;
    virtual void onInput()  = 0;
    virtual void onLoad()   = 0;
    virtual void doRender() = 0;
    virtual void onExit()   = 0;
};

#endif
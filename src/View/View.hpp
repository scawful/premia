#ifndef View_hpp
#define View_hpp

#include "Premia.hpp"
#include "EventInterface.hpp"
#include "SDL.hpp"
#include "ImGui.hpp"
#include "ImPlot.hpp"

class View 
{
public:
    View()=default;
    virtual ~View()=default;

    virtual void addAuth(CRString, CRString) = 0;
    virtual void addEvent(CRString, const Premia::EventHandler &) = 0;
    virtual void addLogger(const Premia::ConsoleLogger &) = 0;
    virtual void update() = 0;
};

#endif
#ifndef View_hpp
#define View_hpp

#include "Premia.hpp"
#include "SDL.hpp"
#include "ImGui.hpp"
#include "ImPlot.hpp"

class View 
{
public:
    View()=default;
    virtual ~View()=default;

    virtual void addEvent(CRString, const EventHandler &) = 0;
    virtual void addLogger(const ConsoleLogger &) = 0;
    virtual void update() = 0;
};

#endif
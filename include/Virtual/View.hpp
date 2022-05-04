#ifndef View_hpp
#define View_hpp

#include "Premia.hpp"
#include "Library/SDL.hpp"
#include "Library/ImGui.hpp"
#include "Library/ImPlot.hpp"
#include "Library/Boost.hpp"

class View 
{
public:
    View()=default;
    virtual ~View()=default;

    virtual String getName() = 0;
    virtual void addEvent(CRString, const EventHandler &) = 0;
    virtual void addLogger(const ConsoleLogger &) = 0;
    virtual void update() = 0;
};

#endif
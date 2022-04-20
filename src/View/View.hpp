#ifndef View_hpp
#define View_hpp

#include "../core.hpp"

class View 
{
public:
    View()=default;
    virtual ~View()=default;

    virtual void addEvent(const VoidEventHandler &) = 0;
    virtual void update() = 0;
};

#endif
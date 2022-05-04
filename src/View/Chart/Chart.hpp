#ifndef Chart_hpp
#define Chart_hpp

#include "Premia.hpp"
#include "Library/ImGui.hpp"
#include "Library/ImPlot.hpp"

class Chart 
{
public:
    Chart()=default;
    virtual ~Chart()=default;

    virtual void update() = 0;
};

#endif
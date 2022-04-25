#ifndef Chart_hpp
#define Chart_hpp

#include "core.hpp"

class Chart 
{
public:
    Chart()=default;
    virtual ~Chart()=default;

    virtual void update() = 0;
};

#endif
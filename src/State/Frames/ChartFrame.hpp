#ifndef ChartFrame_hpp
#define ChartFrame_hpp

#include "../../core.hpp"
#include "Frame.hpp"

class ChartFrame : public Frame 
{
private:

public:
    ChartFrame();

    void update();
    void render();
}

#endif
#ifndef LinePlotFrame_hpp
#define LinePlotFrame_hpp

#include "../../core.hpp"

class LinePlotFrame
{
private:
    bool show_lines;
    bool show_fills;
    float fill_ref;
    int shade_mode;
    double xs1[101], ys1[101], ys2[101], ys3[101];

    void initData();

public:
    LinePlotFrame();

    void update();
    void render();
};

#endif
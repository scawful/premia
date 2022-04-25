#ifndef LinePlotFrame_hpp
#define LinePlotFrame_hpp

#include "core.hpp"

class LinePlotFrame
{
private:
    bool show_lines;
    bool show_fills;
    float fill_ref;
    int shade_mode;
    double *x;
    double *y;

    std::vector<double> numDays;
    std::vector<double> balances;
    std::vector<std::string> dates;

    void initData();

public:
    LinePlotFrame();
    ~LinePlotFrame();

    void update();
    void render();
};

#endif
#ifndef LinePlotChart_hpp
#define LinePlotChart_hpp

#include "../View.hpp"
#include "Boost.hpp"
#include <fstream>

class LinePlotChart : public View
{
private:
    bool show_lines;
    bool show_fills;
    float fill_ref;
    int shade_mode;
    int num = 0;
    double high = 0;
    double low = 0;
    double *x;
    double *y;

    std::vector<double> numDays;
    std::vector<double> balances;
    std::vector<String> dates;
    std::unordered_map<String, Premia::EventHandler> events;

    void initData();

public:
    LinePlotChart();
    ~LinePlotChart() override;

    void update() override;
    void addAuth(CRString key , CRString token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(CRString key, const Premia::EventHandler & event) override;
};

#endif
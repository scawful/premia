#ifndef LinePlotChart_hpp
#define LinePlotChart_hpp

#include "../View.hpp"

class LinePlotChart : public View
{
private:
    bool show_lines;
    bool show_fills;
    float fill_ref;
    int shade_mode;
    int num = 0;
    double high = 0, low = 0;
    double *x;
    double *y;

    std::vector<double> numDays;
    std::vector<double> balances;
    std::vector<std::string> dates;
    std::unordered_map<std::string, VoidEventHandler> events;

    void initData();

public:
    LinePlotChart();
    ~LinePlotChart();

    void update() override;
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
};

#endif
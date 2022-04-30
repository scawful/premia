#ifndef ChartView_hpp
#define ChartView_hpp

#include "../View.hpp"
#include "CandleChart.hpp"

class ChartView : public View
{
private:
    std::unordered_map<std::string, VoidEventHandler> events;
    std::unordered_map<std::string, std::unique_ptr<Chart>> charts;
    ConsoleLogger logger;
    CandleChart candleChart;

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
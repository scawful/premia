#ifndef ChartView_hpp
#define ChartView_hpp

#include "View.hpp"
#include "Subview/CandleChart.hpp"

class ChartView : public View
{
private:
    std::unordered_map<String, EventHandler> events;
    std::unordered_map<String, std::unique_ptr<Chart>> charts;
    ConsoleLogger logger;
    CandleChart candleChart;

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif
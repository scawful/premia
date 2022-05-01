#ifndef ChartView_hpp
#define ChartView_hpp

#include "../View.hpp"
#include "CandleChart.hpp"

class ChartView : public View
{
private:
    std::unordered_map<std::string, Premia::EventHandler> events;
    std::unordered_map<std::string, std::unique_ptr<Chart>> charts;
    Premia::ConsoleLogger logger;
    CandleChart candleChart;

public:
    void addAuth(String key , String token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(String key, const Premia::EventHandler & event) override;
    void update() override;
};

#endif
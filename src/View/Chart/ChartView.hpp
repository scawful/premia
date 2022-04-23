#ifndef ChartView_hpp
#define ChartView_hpp

#include "../View.hpp"
#include "CandleChart.hpp"

class ChartView : public View
{
private:
    std::unordered_map<std::string, VoidEventHandler> events;
    std::unordered_map<std::string, std::unique_ptr<Chart>> charts;

public:
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
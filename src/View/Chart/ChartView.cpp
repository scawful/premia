#include "ChartView.hpp"

void ChartView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

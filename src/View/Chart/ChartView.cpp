#include "ChartView.hpp"

void ChartView::addAuth(const std::string & key , const std::string & token)
{
    
}

void ChartView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void ChartView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

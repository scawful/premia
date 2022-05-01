#include "ChartView.hpp"

void ChartView::addAuth(const std::string & key , const std::string & token)
{
    
}

void ChartView::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void ChartView::addEvent(const std::string & key, const Premia::EventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

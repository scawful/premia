#include "ChartView.hpp"

void ChartView::addAuth(String key , String token)
{
    
}

void ChartView::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void ChartView::addEvent(String key, const Premia::EventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

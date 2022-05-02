#include "ChartView.hpp"

void ChartView::addAuth(CRString key , CRString token)
{
    
}

void ChartView::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void ChartView::addEvent(CRString key, const Premia::EventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

#include "ChartView.hpp"

void ChartView::addLogger(const ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

void ChartView::addEvent(CRString key, const EventHandler & event)
{
    this->events[key] = event;
}

void ChartView::update() 
{
    candleChart.update();
}

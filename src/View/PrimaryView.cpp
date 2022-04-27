#include "PrimaryView.hpp"

void PrimaryView::addEvent(const std::string & key, const VoidEventHandler & event)
{
    this->events[key] = event;
}

void PrimaryView::update() 
{
    if (ImGui::Button("ChartView")) {
        events.at("chartView")();
    }
}

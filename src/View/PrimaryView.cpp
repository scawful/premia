#include "PrimaryView.hpp"

void PrimaryView::addEvent(const VoidEventHandler & event) 
{
    this->primaryEvent = event;
}

void PrimaryView::update() 
{
    ImGui::Text("Primary State");
}

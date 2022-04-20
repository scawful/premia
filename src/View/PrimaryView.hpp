#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"

class PrimaryView: public View 
{
private:
    VoidEventHandler primaryEvent;

public:
    void addEvent(const VoidEventHandler & event) override;
    void update() override;
};

#endif
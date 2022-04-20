#ifndef LoginView_hpp
#define LoginView_hpp

#include "View.hpp"
#include "../Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    VoidEventHandler loginEvent;

    void drawScreen() const;

public:
    void addEvent(const VoidEventHandler & event) override;
    void update() override;
};

#endif
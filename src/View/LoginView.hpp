#ifndef LoginView_hpp
#define LoginView_hpp

#include "View.hpp"
#include "../Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    VoidEventHandler loginEvent;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawScreen() const;

public:
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void update() override;
};

#endif
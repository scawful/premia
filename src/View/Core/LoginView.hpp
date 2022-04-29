#ifndef LoginView_hpp
#define LoginView_hpp

#include "../View.hpp"
#include "../../Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    
    VoidEventHandler loginEvent;
    ConsoleLogger logger;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawScreen() const;

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void update() override;
};

#endif
#ifndef LoginView_hpp
#define LoginView_hpp

#include "Virtual/View.hpp"
#include "Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    
    ConsoleLogger logger;
    std::unordered_map<String, EventHandler> events;

    void drawScreen() const;

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString, const EventHandler &) override;
    void update() override;
};

#endif
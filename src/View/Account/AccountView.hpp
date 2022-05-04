#ifndef AccountView_hpp
#define AccountView_hpp

#include "../View.hpp"
#include "../../Model/Options/OptionsModel.hpp"

class AccountView: public View 
{
private:
    std::unordered_map<String, EventHandler> events;
    ConsoleLogger logger;

    void drawAccountPane();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif
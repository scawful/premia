#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"
#include "Core/MenuView.hpp"

class PrimaryView: public View 
{
private:
    VoidEventHandler primaryEvent;
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    ConsoleLogger logger;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawInfoPane();
    void drawAccountPane();
    void drawScreen();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
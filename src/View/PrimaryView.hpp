#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"
#include "Core/MenuView.hpp"

class PrimaryView: public View 
{
private:
    ConsoleLogger logger;
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    std::unordered_map<String, EventHandler> events;

    void drawInfoPane();
    void drawAccountPane();
    void drawScreen();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif
#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"
#include "Core/MenuView.hpp"

class PrimaryView: public View 
{
private:
    Premia::ConsoleLogger logger;
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    std::unordered_map<std::string, Premia::EventHandler> events;

    void drawInfoPane();
    void drawAccountPane();
    void drawScreen();

public:
    void addAuth(String key , String token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(String key, const Premia::EventHandler & event) override;
    void update() override;
};

#endif
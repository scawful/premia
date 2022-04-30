#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"
#include "Core/MenuView.hpp"

class PrimaryView: public View 
{
private:
    ConsoleLogger logger;
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawInfoPane();
    void drawAccountPane();
    void drawScreen();

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void setLoggedIn(const std::string &, const std::string &);
    void update() override;
};

#endif
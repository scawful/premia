#ifndef PrimaryView_hpp
#define PrimaryView_hpp

#include "View.hpp"
#include "Core/MenuView.hpp"

class PrimaryView: public View 
{
private:
    VoidEventHandler primaryEvent;
    std::shared_ptr<View> menuView = std::make_shared<MenuView>();
    std::unordered_map<std::string, VoidEventHandler> events;

public:
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
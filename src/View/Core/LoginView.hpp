#ifndef LoginView_hpp
#define LoginView_hpp

#include "../View.hpp"
#include "../../Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    
    Premia::TDALoginEvent loginEvent;
    Premia::ConsoleLogger logger;
    std::unordered_map<std::string, Premia::EventHandler> events;

    void drawScreen() const;

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(const std::string &, const Premia::EventHandler &) override;
    void addLoginEvent(const Premia::TDALoginEvent &);
    void update() override;
};

#endif
#ifndef LoginView_hpp
#define LoginView_hpp

#include "../View.hpp"
#include "../../Model/Model.hpp"

class LoginView : public View
{
private:
    bool initialized = false;
    
    TDALoginEvent loginEvent;
    ConsoleLogger logger;
    std::unordered_map<std::string, VoidEventHandler> events;

    void drawScreen() const;

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string &, const VoidEventHandler &) override;
    void addLoginEvent(const TDALoginEvent &);
    void update() override;
};

#endif
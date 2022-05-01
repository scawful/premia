#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include "../View.hpp"
#include "../../Model/Options/OptionsModel.hpp"

class OptionChainView: public View 
{
private:
    std::unordered_map<std::string, Premia::EventHandler> events;
    Premia::ConsoleLogger logger;
    OptionsModel model;

    void drawSearch();
    void drawChain();
    void drawUnderlying();

public:
    void addAuth(String key , String token) override {
        // needs to be taken out of the View header as a virtual routine
    }
    void addLogger(const Premia::ConsoleLogger & logger) override;
    void addEvent(String key, const Premia::EventHandler & event) override;
    void update() override;
};

#endif
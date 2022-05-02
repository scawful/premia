#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include "../View.hpp"
#include "../../Model/Options/OptionsModel.hpp"

class OptionChainView: public View 
{
private:
    std::unordered_map<String, EventHandler> events;
    ConsoleLogger logger;
    OptionsModel model;

    void drawSearch();
    void drawChain();
    void drawUnderlying();

public:
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(CRString key, const EventHandler & event) override;
    void update() override;
};

#endif
#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include "../View.hpp"
#include "../../Model/Options/OptionsModel.hpp"

class OptionChainView: public View 
{
private:
    std::unordered_map<std::string, VoidEventHandler> events;
    ConsoleLogger logger;

    OptionsModel model;

    void drawSearch();
    void drawOptionChain();

public:
    void addAuth(const std::string & key , const std::string & token) override;
    void addLogger(const ConsoleLogger & logger) override;
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
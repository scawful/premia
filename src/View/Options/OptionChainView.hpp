#ifndef OptionChainView_hpp
#define OptionChainView_hpp

#include "../View.hpp"
#include "../../Model/Options/OptionsModel.hpp"

class OptionChainView: public View 
{
private:
    std::unordered_map<std::string, VoidEventHandler> events;

    OptionsModel model;

    void drawSearch();
    void drawOptionChain();

public:
    void addEvent(const std::string & key, const VoidEventHandler & event) override;
    void update() override;
};

#endif
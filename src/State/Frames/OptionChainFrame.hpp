#ifndef OptionChainFrame_hpp
#define OptionChainFrame_hpp

#include "Frame.hpp"

class OptionChainFrame : public Frame
{
private:
    tda::OptionChain optionChainData;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;

public:
    OptionChainFrame();

    void init_chain(std::string ticker);
    void update();
};

#endif
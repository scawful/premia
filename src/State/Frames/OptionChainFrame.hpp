#ifndef OptionChainFrame_hpp
#define OptionChainFrame_hpp

#include "Frame.hpp"

class OptionChainFrame : public Frame
{
private:
    tda::OptionChain optionChainData;
    std::vector<tda::OptionsDateTimeObj> optionsDateTimeObj;
    std::vector<const char*> datetime_array;

    bool isActive;

    void draw_search();
    void draw_option_chain();

public:
    OptionChainFrame();

    void load_data();
    void update();
};

#endif
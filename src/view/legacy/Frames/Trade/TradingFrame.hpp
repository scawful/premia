#ifndef TradingFrame_hpp
#define TradingFrame_hpp

#include "../Frame.hpp"

class TradingFrame : public Frame 
{
private:
    std::map<String, tda::Quote> quotes;

public:
    TradingFrame();

    void update();
};

#endif
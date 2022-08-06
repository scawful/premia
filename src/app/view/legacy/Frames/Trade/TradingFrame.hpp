#ifndef TradingFrame_hpp
#define TradingFrame_hpp

#include "../Frame.hpp"

class TradingFrame : public Frame 
{
private:
    std::map<std::string, tda::Quote> quotes;

public:
    TradingFrame();

    void Update();
};

#endif
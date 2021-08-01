#ifndef PricingStructures_hpp
#define PricingStructures_hpp

#include "../../core.hpp"

namespace tda 
{
    struct Candle
    {
        int volume;
        std::pair<double, double> highLow;
        std::pair<double, double> openClose;     
        std::string datetime;
        time_t raw_datetime;
    };

    struct OptionsContract
    {
        std::string putCall;
        double bid, ask, bidSize, askSize;
        double open, close, high, low;
        double delta, gamma, theta, vega, rho;
    };

    struct StrikePriceMap
    {
        std::string strikePrice;
        std::unordered_map<std::string, std::string> raw_option;
        std::unordered_map<std::string, OptionsContract> options;
    };

    struct OptionsDateTimeObj
    {
        std::string datetime;
        std::vector<StrikePriceMap> strikePriceObj;
    };
}

#endif
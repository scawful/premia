#ifndef PricingStructures_hpp
#define PricingStructures_hpp

#include "../../../core.hpp"

namespace tda 
{
    struct Candle
    {
        double volume;
        double high, low;
        double open, close;
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

    enum PeriodType
    {
        DAY,
        MONTH,
        YEAR,
        YTD
    };

    enum FrequencyType
    {
        MINUTE,
        DAILY,
        WEEKLY,
        MONTHLY
    };

    enum OrderType
    {
        MARKET,
        LIMIT,
        STOP,
        STOP_LIMIT,
        TRAILING_STOP,
        MARKET_ON_CLOSE,
        EXERCISE,
        TRAILING_STOP_LIMIT,
        NET_DEBIT,
        NET_CREDIT,
        NET_ZERO
    };


}

#endif
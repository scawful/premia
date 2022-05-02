#ifndef PricingStructures_hpp
#define PricingStructures_hpp

#include "Premia.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace tda 
{
    struct Candle
    {
        double volume;
        double high;
        double low;
        double open;
        double close;
        String datetime;
        time_t raw_datetime;
    };

    struct OptionsContract
    {
        String putCall;
        double bid;
        double ask;
        double bidSize;
        double askSize;
        double open, close, high, low;
        double delta, gamma, theta, vega, rho;
    };

    struct StrikePriceMap
    {
        String strikePrice;
        StringMap raw_option;
        std::unordered_map<String, OptionsContract> options;
    };

    struct OptionsDateTimeObj
    {
        String datetime;
        ArrayList<StrikePriceMap> strikePriceObj;
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
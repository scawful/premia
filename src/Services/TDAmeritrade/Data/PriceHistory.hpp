#ifndef PriceHistory_hpp
#define PriceHistory_hpp

#include "PricingStructures.hpp"

namespace tda 
{
    // //CandleList:
    // {
    // "candles": [
    //     {
    //     "close": 0,
    //     "datetime": 0,
    //     "high": 0,
    //     "low": 0,
    //     "open": 0,
    //     "volume": 0
    //     }
    // ],
    // "empty": false,
    // "symbol": "string"
    // }
    class PriceHistory
    {
    private:
        std::vector<tda::Candle> minute_candles;
        std::vector<tda::Candle> daily_candles;
        std::vector<tda::Candle> weekly_candles;
        std::vector<tda::Candle> monthly_candles;

        boost::property_tree::ptree priceHistoryData;
        std::map<std::string, std::string> priceHistoryVariables;
        std::map<std::string, std::string> candleData;
        std::vector< tda::Candle > candleVector;

        void initVariables();
            
    public:
        PriceHistory();
        PriceHistory(boost::property_tree::ptree price_history_data);

        void addCandle(tda::Candle candle);
        std::vector< tda::Candle > getCandleVector();
        std::string getCandleDataVariable( std::string variable );
        std::string getPriceHistoryVariable( std::string variable );
        void setPriceHistoryVariable(std::string key, std::string value);

        void clear();
    };
}

#endif
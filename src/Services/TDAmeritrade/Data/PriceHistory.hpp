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
        bool initialized;
        std::string tickerSymbol;
        std::vector<std::vector<tda::Candle>> candles;

        std::unordered_map<std::string, std::string> priceHistoryVariables;
        std::unordered_map<std::string, std::string> candleData;
        std::vector< tda::Candle > candleVector;
            
    public:
        PriceHistory();

        void addCandle(tda::Candle candle);
        void addCandleByType(Candle candle, int type);

        std::vector<tda::Candle> getCandles(int index);
        int getNumCandles(int index);

        std::vector< tda::Candle > getCandleVector();
        std::string getCandleDataVariable( std::string variable );
        std::string getPriceHistoryVariable( std::string variable );
        std::string getTickerSymbol();
        bool getInitialized();

        void setPriceHistoryVariable(std::string key, std::string value);
        void setTickerSymbol(std::string ticker);
        void setInitialized();

        void updatePriceHistory();

        void clear();
    };
}

#endif
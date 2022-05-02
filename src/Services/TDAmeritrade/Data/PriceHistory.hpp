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
        String tickerSymbol;
        std::vector<std::vector<tda::Candle>> candles;

        StringMap priceHistoryVariables;
        StringMap candleData;
        std::vector< tda::Candle > candleVector;
            
    public:
        PriceHistory();

        void addCandle(tda::Candle candle);
        void addCandleByType(Candle candle, int type);

        std::vector<tda::Candle> getCandles(int index);
        int getNumCandles(int index);

        std::vector< tda::Candle > getCandleVector();
        String getCandleDataVariable( String variable );
        String getPriceHistoryVariable( String variable );
        String getTickerSymbol();
        bool getInitialized();

        void setPriceHistoryVariable(String key, String value);
        void setTickerSymbol(String ticker);
        void setInitialized();

        void updatePriceHistory();

        void clear();
    };
}

#endif
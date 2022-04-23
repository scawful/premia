#ifndef ChartModel_hpp
#define ChartModel_hpp

#include "../Model.hpp"

class ChartModel: public Model
{
private:
    bool active = false;
    std::string tickerSymbol;
    tda::Quote quote;
    tda::PriceHistory priceHistory;
    std::vector<tda::Candle> candles;

public:
    bool isActive() const;
    tda::Quote & getQuote();
    int getNumCandles() const;
    tda::Candle getCandle(int i);
    std::string getTickerSymbol() const;
    const std::string getQuoteDetails();
    void fetchPriceHistory(const std::string & ticker, tda::PeriodType ptype, int period_amt, tda::FrequencyType ftype, int freq_amt, bool ext);

};

#endif 
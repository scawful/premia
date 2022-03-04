#ifndef Quote_hpp
#define Quote_hpp

#include "../../../core.hpp"

namespace tda 
{
    class Quote 
    {
    private:
        boost::property_tree::ptree quoteData;
        std::map<std::string, std::string> quoteVariables;

        std::string symbol;
        std::string description;
        double bidPrice, askPrice, lastPrice, openPrice, highPrice, lowPrice, closePrice;
        double bidSize, askSize, lastSize, bidId, askId, lastId;
        double netChange, totalVolume;
        double quoteTimeInLong, tradeTimeInLong;
        double mark;
        std::string exchange;
        std::string exchangeView;
        bool marginable;
        bool shortable;
        double digits;
        double _52WkHigh, _52WkLow;
        double peRatio;
        double divAmount, divYield;
        std::string divDate;
        std::string securityStatus;
        double regularMarketLastPrice, regularMarketLastSize, regularMarketNetChange, regularMarketTradeTimeInLong;

    public:
        Quote();

        void setQuoteVariable(std::string key, std::string value);
        std::string getQuoteVariable(std::string variable);

        void clear();
    };
}

#endif
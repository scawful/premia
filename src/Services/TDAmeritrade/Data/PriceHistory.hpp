#ifndef PriceHistory_hpp
#define PriceHistory_hpp

#include "PricingStructures.hpp"

namespace tda 
{
    class PriceHistory
    {
    private:
        boost::property_tree::ptree priceHistoryData;
        std::map<std::string, std::string> priceHistoryVariables;
        std::map<std::string, std::string> candleData;
        std::vector< tda::Candle > candleVector;

        void initVariables();
            
    public:
        PriceHistory();
        PriceHistory(boost::property_tree::ptree price_history_data);

        std::vector< tda::Candle > getCandleVector();
        std::string getCandleDataVariable( std::string variable );
        std::string getPriceHistoryVariable( std::string variable );
    };
}

#endif
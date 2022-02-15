#ifndef OptionChain_hpp
#define OptionChain_hpp

#include "PricingStructures.hpp"

namespace tda
{
    class OptionChain
    {
    private:
        boost::property_tree::ptree optionChainData;

        std::unordered_map<std::string, std::string> callExpDateMap;
        std::unordered_map<std::string, std::string> putExpDateMap;
        std::unordered_map<std::string, std::string> optionChainMap;
        std::unordered_map<std::string, std::string> underlyingMap;
        std::vector<OptionsDateTimeObj> optionsDateTimeVector;

        void initVariables();

    public:
        OptionChain();
        OptionChain( boost::property_tree::ptree option_chain_data );

        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj();
        std::string getCallVariable( std::string variable );
        std::string getPutVariable( std::string variable );
        std::string getOptionChainDataVariable( std::string variable );
        std::string getUnderlyingDataVariable( std::string variable );

    };
}

#endif
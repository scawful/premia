#ifndef OptionChain_hpp
#define OptionChain_hpp

#include "../../core.hpp"
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
        std::vector<OptionsDateTimeObj> optionsDateTimeVector;

        void initVariables();

    public:
        OptionChain( boost::property_tree::ptree option_chain_data );

        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj();
        std::string getCallVariable( std::string variable );
        std::string getPutVariable( std::string variable );
        std::string getOptionChainDataVariable( std::string variable );

    };
}

#endif
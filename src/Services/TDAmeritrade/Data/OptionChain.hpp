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

    public:
        OptionChain();

        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj() const;
        std::string getCallVariable( const std::string & variable );
        std::string getPutVariable( const std::string & variable );
        std::string getOptionChainDataVariable( const std::string & variable );
        std::string getUnderlyingDataVariable( const std::string & variable );

        void addOptionsDateTimeObj( const OptionsDateTimeObj & obj );
        void setCallVariable( const std::string & key, const std::string & value );
        void setPutVariable( const std::string & key, const std::string & value );
        void setOptionChainVariable( const std::string & key, const std::string & value );
        void setUnderlyingVariable( const std::string & key, const std::string & value );

    };
}

#endif
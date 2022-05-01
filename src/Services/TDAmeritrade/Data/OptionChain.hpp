#ifndef OptionChain_hpp
#define OptionChain_hpp

#include "PricingStructures.hpp"

namespace tda
{
    class OptionChain
    {
    private:
        std::unordered_map<std::string, std::string> callExpDateMap;
        std::unordered_map<std::string, std::string> putExpDateMap;
        std::unordered_map<std::string, std::string> optionChainMap;
        std::unordered_map<std::string, std::string> underlyingMap;
        std::vector<OptionsDateTimeObj> callOptionArray;
        std::vector<OptionsDateTimeObj> putOptionArray;
        std::vector<OptionsDateTimeObj> optionsDateTimeVector;

    public:
        OptionChain();

        std::vector<OptionsDateTimeObj> getCallOptionArray() const;
        std::vector<OptionsDateTimeObj> getPutOptionArray() const;
        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj() const;
        std::string getCallVariable( String variable );
        std::string getPutVariable( String variable );
        std::string getOptionChainDataVariable( String variable );
        std::string getUnderlyingDataVariable( String variable );



        void addOptionsDateTimeObj( const OptionsDateTimeObj & obj );
        void addOptionsDateTimeObj( const OptionsDateTimeObj & obj, int idx );
        void setCallVariable( String key, String value );
        void setPutVariable( String key, String value );
        void setOptionChainVariable( String key, String value );
        void setUnderlyingVariable( String key, String value );

    };
}

#endif
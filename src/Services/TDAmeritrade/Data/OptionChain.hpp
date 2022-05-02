#ifndef OptionChain_hpp
#define OptionChain_hpp

#include "PricingStructures.hpp"

namespace tda
{
    class OptionChain
    {
    private:
        StringMap callExpDateMap;
        StringMap putExpDateMap;
        StringMap optionChainMap;
        StringMap underlyingMap;
        std::vector<OptionsDateTimeObj> callOptionArray;
        std::vector<OptionsDateTimeObj> putOptionArray;
        std::vector<OptionsDateTimeObj> optionsDateTimeVector;

    public:
        OptionChain();

        std::vector<OptionsDateTimeObj> getCallOptionArray() const;
        std::vector<OptionsDateTimeObj> getPutOptionArray() const;
        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj() const;
        String getCallVariable( CRString variable );
        String getPutVariable( CRString variable );
        String getOptionChainDataVariable( CRString variable );
        String getUnderlyingDataVariable( CRString variable );



        void addOptionsDateTimeObj( const OptionsDateTimeObj & obj );
        void addOptionsDateTimeObj( const OptionsDateTimeObj & obj, int idx );
        void setCallVariable( CRString key, CRString value );
        void setPutVariable( CRString key, CRString value );
        void setOptionChainVariable( CRString key, CRString value );
        void setUnderlyingVariable( CRString key, CRString value );

    };
}

#endif
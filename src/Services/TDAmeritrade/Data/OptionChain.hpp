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
        ArrayList<OptionsDateTimeObj> callOptionArray;
        ArrayList<OptionsDateTimeObj> putOptionArray;
        ArrayList<OptionsDateTimeObj> optionsDateTimeVector;

    public:
        OptionChain();

        ArrayList<OptionsDateTimeObj> getCallOptionArray() const;
        ArrayList<OptionsDateTimeObj> getPutOptionArray() const;
        ArrayList<OptionsDateTimeObj> getOptionsDateTimeObj() const;
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
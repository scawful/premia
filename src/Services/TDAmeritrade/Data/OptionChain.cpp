#include "OptionChain.hpp"

using namespace tda;

OptionChain::OptionChain()=default;

ArrayList<OptionsDateTimeObj> OptionChain::getOptionsDateTimeObj() const
{
    return optionsDateTimeVector;
}

String OptionChain::getCallVariable(CRString variable)
{
    return callExpDateMap[ variable ];
}

String OptionChain::getPutVariable(CRString variable)
{
    return putExpDateMap[ variable ];
}

String OptionChain::getOptionChainDataVariable(CRString variable)
{
    return optionChainMap[ variable ];
}

String OptionChain::getUnderlyingDataVariable(CRString variable)
{
    return underlyingMap[ variable ];
}

void OptionChain::addOptionsDateTimeObj(const OptionsDateTimeObj & obj)
{
    optionsDateTimeVector.push_back(obj);
}

ArrayList<OptionsDateTimeObj> OptionChain::getCallOptionArray() const
{
    return callOptionArray;
}

ArrayList<OptionsDateTimeObj> OptionChain::getPutOptionArray() const
{
    return putOptionArray;
}

void OptionChain::addOptionsDateTimeObj( const OptionsDateTimeObj & obj, int idx )
{
    if (idx) {
        callOptionArray.push_back(obj);
    } else {
        putOptionArray.push_back(obj);
    }
}

void OptionChain::setCallVariable(CRString key, CRString value)
{
    callExpDateMap[key] = value;
}

void OptionChain::setPutVariable(CRString key, CRString value)
{
    putExpDateMap[key] = value;
}

void OptionChain::setOptionChainVariable(CRString key, CRString value)
{
    optionChainMap[key] = value;
}

void OptionChain::setUnderlyingVariable(CRString key, CRString value)
{
    underlyingMap[key] = value;
}
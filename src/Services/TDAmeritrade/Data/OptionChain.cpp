#include "OptionChain.hpp"

using namespace tda;

OptionChain::OptionChain()=default;

std::vector<OptionsDateTimeObj> OptionChain::getOptionsDateTimeObj() const
{
    return optionsDateTimeVector;
}

std::string OptionChain::getCallVariable(String variable)
{
    return callExpDateMap[ variable ];
}

std::string OptionChain::getPutVariable(String variable)
{
    return putExpDateMap[ variable ];
}

std::string OptionChain::getOptionChainDataVariable(String variable)
{
    return optionChainMap[ variable ];
}

std::string OptionChain::getUnderlyingDataVariable(String variable)
{
    return underlyingMap[ variable ];
}

void OptionChain::addOptionsDateTimeObj(const OptionsDateTimeObj & obj)
{
    optionsDateTimeVector.push_back(obj);
}

std::vector<OptionsDateTimeObj> OptionChain::getCallOptionArray() const
{
    return callOptionArray;
}

std::vector<OptionsDateTimeObj> OptionChain::getPutOptionArray() const
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

void OptionChain::setCallVariable(String key, String value)
{
    callExpDateMap[key] = value;
}

void OptionChain::setPutVariable(String key, String value)
{
    putExpDateMap[key] = value;
}

void OptionChain::setOptionChainVariable(String key, String value)
{
    optionChainMap[key] = value;
}

void OptionChain::setUnderlyingVariable(String key, String value)
{
    underlyingMap[key] = value;
}
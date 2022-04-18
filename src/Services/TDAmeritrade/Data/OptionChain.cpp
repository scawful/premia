#include "OptionChain.hpp"

using namespace tda;

OptionChain::OptionChain()=default;

std::vector<OptionsDateTimeObj> OptionChain::getOptionsDateTimeObj() const
{
    return optionsDateTimeVector;
}

std::string OptionChain::getCallVariable(const std::string & variable)
{
    return callExpDateMap[ variable ];
}

std::string OptionChain::getPutVariable(const std::string & variable)
{
    return putExpDateMap[ variable ];
}

std::string OptionChain::getOptionChainDataVariable(const std::string & variable)
{
    return optionChainMap[ variable ];
}

std::string OptionChain::getUnderlyingDataVariable(const std::string & variable)
{
    return underlyingMap[ variable ];
}

void OptionChain::addOptionsDateTimeObj(const OptionsDateTimeObj & obj)
{
    optionsDateTimeVector.push_back(obj);
}

void OptionChain::setCallVariable(const std::string & key, const std::string & value)
{
    callExpDateMap[key] = value;
}

void OptionChain::setPutVariable(const std::string & key, const std::string & value)
{
    putExpDateMap[key] = value;
}

void OptionChain::setOptionChainVariable(const std::string & key, const std::string & value)
{
    optionChainMap[key] = value;
}

void OptionChain::setUnderlyingVariable(const std::string & key, const std::string & value)
{
    underlyingMap[key] = value;
}
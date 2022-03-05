#include "OptionChain.hpp"

using namespace tda;

OptionChain::OptionChain()
{
    
}

std::vector<OptionsDateTimeObj> OptionChain::getOptionsDateTimeObj()
{
    return optionsDateTimeVector;
}

std::string OptionChain::getCallVariable( std::string variable )
{
    return callExpDateMap[ variable ];
}

std::string OptionChain::getPutVariable( std::string variable )
{
    return putExpDateMap[ variable ];
}

std::string OptionChain::getOptionChainDataVariable( std::string variable )
{
    return optionChainMap[ variable ];
}

std::string OptionChain::getUnderlyingDataVariable( std::string variable )
{
    return underlyingMap[ variable ];
}

void OptionChain::addOptionsDateTimeObj( OptionsDateTimeObj obj )
{
    optionsDateTimeVector.push_back(obj);
}

void OptionChain::setCallVariable( std::string key, std::string value )
{
    callExpDateMap[key] = value;
}

void OptionChain::setPutVariable( std::string key, std::string value )
{
    putExpDateMap[key] = value;
}

void OptionChain::setOptionChainVariable( std::string key, std::string value )
{
    optionChainMap[key] = value;
}

void OptionChain::setUnderlyingVariable( std::string key, std::string value )
{
    underlyingMap[key] = value;
}
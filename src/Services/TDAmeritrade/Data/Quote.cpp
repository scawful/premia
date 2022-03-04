#include "Quote.hpp"

using namespace tda;

Quote::Quote()
{
    
}


void Quote::setQuoteVariable(std::string key, std::string value)
{
    quoteVariables[key] = value;
}

std::string Quote::getQuoteVariable( std::string variable )
{
    return quoteVariables[variable];
}

void Quote::clear()
{
    quoteVariables.clear();
}
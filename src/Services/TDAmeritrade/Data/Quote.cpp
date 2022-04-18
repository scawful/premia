#include "Quote.hpp"

using namespace tda;

Quote::Quote()=default;


void Quote::setQuoteVariable(const std::string & key, const std::string & value)
{
    quoteVariables[key] = value;
}

std::string Quote::getQuoteVariable(const std::string & variable)
{
    return quoteVariables[variable];
}

void Quote::clear()
{
    quoteVariables.clear();
}
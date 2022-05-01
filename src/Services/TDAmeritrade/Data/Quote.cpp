#include "Quote.hpp"

using namespace tda;

Quote::Quote()=default;


void Quote::setQuoteVariable(String key, String value)
{
    quoteVariables[key] = value;
}

std::string Quote::getQuoteVariable(String variable)
{
    return quoteVariables[variable];
}

void Quote::clear()
{
    quoteVariables.clear();
}
#include "Quote.hpp"

using namespace tda;

void Quote::initVariables()
{
    for (auto & array_element: quoteData ) 
    {
        for (auto & property: array_element.second) 
        {
            quoteVariables[ property.first ] = property.second.get_value < std::string > ();
        }
    }
}

Quote::Quote()
{
    
}

Quote::Quote( boost::property_tree::ptree quote_data )
{
    quoteData = quote_data;
    initVariables();
}

void Quote::updateTree( boost::property_tree::ptree quote_data )
{
    quoteData = quote_data;
    initVariables();
}

std::string Quote::getQuoteVariable( std::string variable )
{
    return quoteVariables[variable];
}

void Quote::clear()
{
    quoteVariables.clear();
}
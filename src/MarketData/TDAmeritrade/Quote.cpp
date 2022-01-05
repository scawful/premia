#include "Quote.hpp"

/* =============== Quote Class =============== */

void tda::Quote::initVariables()
{
    for (auto & array_element: quoteData ) 
    {
        for (auto & property: array_element.second) 
        {
            quoteVariables[ property.first ] = property.second.get_value < std::string > ();
        }
    }
}

tda::Quote::Quote()
{
    
}

tda::Quote::Quote( boost::property_tree::ptree quote_data )
{
    quoteData = quote_data;
    initVariables();
}

void tda::Quote::updateTree( boost::property_tree::ptree quote_data )
{
    quoteData = quote_data;
    initVariables();
}

std::string tda::Quote::getQuoteVariable( std::string variable )
{
    return quoteVariables[variable];
}
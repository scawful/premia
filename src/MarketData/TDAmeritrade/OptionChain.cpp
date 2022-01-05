#include "OptionChain.hpp"

/* =============== OptionChain Class =============== */

void tda::OptionChain::initVariables()
{
    for ( auto &option_it: optionChainData )
    {
        if ( option_it.first == "callExpDateMap" )
        {
            for ( auto &date_it: option_it.second )
            {
                OptionsDateTimeObj options_dt_obj;
                options_dt_obj.datetime = date_it.first;

                for ( auto &strike_it: date_it.second )
                {
                    StrikePriceMap imported_strike;
                    imported_strike.strikePrice = strike_it.first;

                    for ( auto &contract_it: strike_it.second )
                    {
                        //std::cout << details_it.first << " :: " << details_it.second.get_value< std::string >() << std::endl;
                        for ( auto &details_it: contract_it.second )
                        {
                            imported_strike.raw_option[ details_it.first ] = details_it.second.get_value< std::string >();
                        }

                        options_dt_obj.strikePriceObj.push_back( imported_strike );
                    }
                }

                optionsDateTimeVector.push_back( options_dt_obj );
            }
        }
        
        if ( option_it.first == "putExpDateMap" )
        {
            for ( auto &date_it: option_it.second )
            {
                
            }
        }

        if ( option_it.first == "underlying" )
        {
            for ( auto &underlying_it: option_it.second )
            {
                underlyingMap[ underlying_it.first ] = underlying_it.second.get_value<std::string>();
            }
        }

        optionChainMap[ option_it.first ] = option_it.second.get_value< std::string >();
    }
}

tda::OptionChain::OptionChain()
{
    
}

tda::OptionChain::OptionChain( boost::property_tree::ptree option_chain_data )
{
    optionChainData = option_chain_data;
    initVariables();
}

std::vector<tda::OptionsDateTimeObj> tda::OptionChain::getOptionsDateTimeObj()
{
    return optionsDateTimeVector;
}

std::string tda::OptionChain::getCallVariable( std::string variable )
{
    return callExpDateMap[ variable ];
}

std::string tda::OptionChain::getPutVariable( std::string variable )
{
    return putExpDateMap[ variable ];
}

std::string tda::OptionChain::getOptionChainDataVariable( std::string variable )
{
    return optionChainMap[ variable ];
}

std::string tda::OptionChain::getUnderlyingDataVariable( std::string variable )
{
    return underlyingMap[ variable ];
}
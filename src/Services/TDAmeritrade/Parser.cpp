#include "Parser.hpp"

tda::Parser::Parser() 
{
    
}

std::vector<tda::Watchlist> tda::Parser::parse_watchlist_data(boost::property_tree::ptree data) 
{
    std::vector<tda::Watchlist> watchlists;
    // JSON Outer Layer 
    for ( auto & each_data : data ) 
    {
        // Array of Watchlists
        Watchlist watchlist;
        for ( auto & each_watchlist : each_data.second )
        {
            // Each element in a watchlist 
            if ( each_watchlist.first == "name" ) {
                watchlist.setName(each_watchlist.second.get_value< std::string >());
            } else if ( each_watchlist.first == "watchlistId" ) {
                watchlist.setId(each_watchlist.second.get_value< int >());
            } else if ( each_watchlist.first == "accountId" ) {
                // account_d = each_element.second.get_value< std::string >();
            } else {
                for ( auto & each_element : each_watchlist.second ) 
                {
                    // Watchlist items 
                    for ( auto & each_item : each_element.second ) 
                    {
                        // Instrument sub array 
                        if ( each_item.first == "instrument" ) {
                            std::string symbol, desc, type;
                            for ( auto & each : each_item.second ) {
                                if ( each.first == "symbol" ) {
                                    symbol = each.second.get_value< std::string >();
                                } else if ( each.first == "description" ) {
                                    desc = each.second.get_value< std::string >();
                                } else if ( each.first == "assetType" ) {
                                    type = each.second.get_value< std::string >();
                                }
                            }
                            watchlist.addInstrument(symbol, desc, type);
                        } else {
                            watchlist.addVariable(each_item.first, each_item.second.get_value< std::string >());
                        }
                    }
                }
            }
        }
        watchlists.push_back(watchlist);
    }
    
    return watchlists;
}

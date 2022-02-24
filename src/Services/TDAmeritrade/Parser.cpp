#include "Parser.hpp"

tda::Parser::Parser() 
{
    
}

JSONObject::ptree tda::Parser::read_response(std::string response)
{
    std::istringstream json_response(response);
    JSONObject::ptree property_tree;

    try {
        read_json(json_response, property_tree);
    }
    catch (std::exception &json_parser_error) {
        SDL_Log("%s", json_parser_error.what());
    }

    return property_tree;
}

UserPrincipals tda::Parser::parse_user_principals( boost::property_tree::ptree data )
{
    UserPrincipals user_principals;

    std::unordered_map<std::string, std::string> account_data;
    BOOST_FOREACH (JSONObject::ptree::value_type &v, user_principals.get_child("accounts."))
    {
        for (auto &acct_it : v.second)
        {
            account_data[acct_it.first] = acct_it.second.get_value<std::string>();
        }
        break;
    }
    user_principals.set_account_data(account_data);
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

#include "Parser.hpp"

using namespace tda;

Parser::Parser() 
{
    
}

JSONObject::ptree Parser::read_response(std::string response)
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

std::string Parser::parse_access_token(std::string response)
{
    std::string access_token;
    JSONObject::ptree data = read_response(response);
    for (auto& access_it : data) {
        if (access_it.first == "access_token") {
            access_token = access_it.second.get_value<std::string>();
        }
    }
    return access_token;
}

Quote Parser::parse_quote(JSONObject::ptree data)
{
    Quote quote;

    for (auto& array_element : data) {
        for (auto& property : array_element.second) {
            quote.setQuoteVariable(property.first, property.second.get_value<std::string>());
        }
    }

    return quote;
}

PriceHistory Parser::parse_price_history(boost::property_tree::ptree data, std::string ticker, int freq)
{
    PriceHistory price_history;
    price_history.setTickerSymbol(ticker);

    for (auto& history_it : data) {
        if (history_it.first == "candles") {
            for (auto& candle_it : history_it.second) {
                tda::Candle newCandle;
                std::string datetime;
                std::pair<double, double> high_low;
                std::pair<double, double> open_close;

                for (auto& candle2_it : candle_it.second) {
                    try {
                        if ( candle2_it.first == "high" ) {
                            high_low.first = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                            newCandle.high = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                        }

                        if ( candle2_it.first == "low" ) {
                            high_low.second = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                            newCandle.low = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                        }
                        
                        if ( candle2_it.first == "open" ) {
                            open_close.first = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                            newCandle.open = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                        }
                        if ( candle2_it.first == "close" ) {
                            open_close.second = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                            newCandle.close = boost::lexical_cast<double>(candle2_it.second.get_value<std::string> () );
                        }

                        if ( candle2_it.first == "volume" ) {
                            newCandle.volume = boost::lexical_cast<double>( candle2_it.second.get_value<std::string> () );
                        }
                    } 
                    catch (boost::wrapexcept<boost::bad_lexical_cast>& e) {
                        std::cout << "parse_price_history:: " << e.what() << std::endl;
                    }


                    if (candle2_it.first == "datetime")
                    {
                        std::stringstream dt_ss;
                        std::time_t secsSinceEpoch = boost::lexical_cast<std::time_t>(candle2_it.second.get_value<std::string>());
                        newCandle.raw_datetime = secsSinceEpoch;
                        secsSinceEpoch *= 0.001;

                        //%a %d %b %Y - %I:%M:%S%p
                        //%H:%M:%S
                        dt_ss << std::put_time(std::localtime(&secsSinceEpoch), "%a %d %b %Y - %I:%M:%S%p");
                        datetime = dt_ss.str();
                        //datetime = boost::lexical_cast<std::string>(candle2_it.second.get_value<std::string> ());
                    }

                }

                newCandle.datetime = datetime;
                newCandle.highLow = high_low;
                newCandle.openClose = open_close;

                price_history.addCandleByType(newCandle, freq);
                price_history.addCandle(newCandle);
            }
        }
    }
    price_history.setInitialized();
    return price_history;
}

UserPrincipals Parser::parse_user_principals( boost::property_tree::ptree data )
{
    UserPrincipals user_principals;

    BOOST_FOREACH (JSONObject::ptree::value_type &v, data.get_child("accounts.")) 
    {
        std::unordered_map<std::string, std::string> account_data;
        for (auto &acct_it : v.second)
        {
            account_data[acct_it.first] = acct_it.second.get_value<std::string>();
        }
        user_principals.add_account_data(account_data);
    }
    user_principals.set_account_data(user_principals.get_account_data_array(0));

    return user_principals;
}

OptionChain Parser::parse_option_chain(JSONObject::ptree data)
{
    OptionChain optionChain;
    for (auto &option_it: data) {
        if (option_it.first == "callExpDateMap") {
            for (auto &date_it: option_it.second) {
                OptionsDateTimeObj options_dt_obj;
                options_dt_obj.datetime = date_it.first;

                for (auto &strike_it: date_it.second) {
                    StrikePriceMap imported_strike;
                    imported_strike.strikePrice = strike_it.first;

                    for (auto &contract_it: strike_it.second) {
                        //std::cout << details_it.first << " :: " << details_it.second.get_value< std::string >() << std::endl;
                        for (auto &details_it: contract_it.second) {
                            imported_strike.raw_option[ details_it.first ] = details_it.second.get_value< std::string >();
                        }
                        options_dt_obj.strikePriceObj.push_back( imported_strike );
                    }
                }
                optionChain.addOptionsDateTimeObj(options_dt_obj);
            }
        }
        
        if (option_it.first == "putExpDateMap") {
            for ( auto &date_it: option_it.second ) {
                
            }
        }

        if ( option_it.first == "underlying" ) {
            for ( auto &underlying_it: option_it.second ) {
                optionChain.setUnderlyingVariable(underlying_it.first, underlying_it.second.get_value<std::string>());
            }
        }

        optionChain.setOptionChainVariable(option_it.first, option_it.second.get_value< std::string >());
    }
    return optionChain;
}

Account Parser::parse_account(JSONObject::ptree data)
{
    Account account;
    for (auto& class_it : data) {
        for (auto& account_it : class_it.second) {
            if (account_it.first == "positions") {
                for (auto& position_list_it : account_it.second) {
                    tda::PositionBalances new_position_balance;

                    for (auto& positions_it : position_list_it.second) {
                        new_position_balance.balances[positions_it.first] = positions_it.second.get_value<std::string>();

                        std::unordered_map<std::string, std::string> pos_field;
                        std::unordered_map<std::string, std::string> instrument;

                        for (auto& field_it : positions_it.second) {
                            if (field_it.first == "symbol") {
                                new_position_balance.symbol = field_it.second.get_value<std::string>();
                            }

                            pos_field[field_it.first] = field_it.second.get_value<std::string>();
                        }
                        account.add_position(pos_field);
                    }
                    account.add_balance(new_position_balance);
                }
            } else if (account_it.first == "currentBalances") {
                for (auto& balance_it : account_it.second) {
                    account.set_balance_variable(balance_it.first, balance_it.second.get_value<std::string>());
                }
            } else {
                account.set_account_variable(account_it.first, account_it.second.get_value<std::string>());
            }
        }
    }
    return account;
}

std::vector<tda::Watchlist> tda::Parser::parse_watchlist_data(boost::property_tree::ptree data) 
{
    std::vector<tda::Watchlist> watchlists;
    // JSON Outer Layer 
    for ( auto & each_data : data ) {
        // Array of Watchlists
        Watchlist watchlist;
        for ( auto & each_watchlist : each_data.second ) {
            // Each element in a watchlist 
            if ( each_watchlist.first == "name" ) {
                watchlist.setName(each_watchlist.second.get_value< std::string >());
            } else if ( each_watchlist.first == "watchlistId" ) {
                watchlist.setId(each_watchlist.second.get_value< int >());
            } else if ( each_watchlist.first == "accountId" ) {
                // account_d = each_element.second.get_value< std::string >();
            } else {
                for ( auto & each_element : each_watchlist.second ) { // Watchlist items 
                    for ( auto & each_item : each_element.second ) { // Instrument sub array 
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

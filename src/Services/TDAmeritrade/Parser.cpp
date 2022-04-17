#include "Parser.hpp"

using namespace tda;

Parser::Parser()=default;

/**
 * @brief Take a response from the API as argument and read it into a boost::property_tree
 * @author @scawful
 * 
 * @param response 
 * @return JSONObject::ptree 
 */
JSONObject::ptree Parser::read_response(const std::string & response) const
{
    std::istringstream json_response(response);
    JSONObject::ptree property_tree;

    try {
        read_json(json_response, property_tree);
    } catch (JSONObject::ptree_error const & json_parser_error) {
        SDL_Log("Parser::read_response: %s", json_parser_error.what());
    }

    return property_tree;
}

/**
 * @brief Parse the access token from a response 
 * @author @scawful
 * 
 * @param response 
 * @return std::string 
 */
std::string Parser::parse_access_token(const std::string & response) const
{
    std::string access_token;
    JSONObject::ptree data = read_response(response);
    for (const auto & [key, value] : data) {
        if (key == "access_token") {
            access_token = value.get_value<std::string>();
        }
    }
    return access_token;
}

/**
 * @brief Parse a Quote from the API
 * @author @scawful
 * 
 * @param data 
 * @return Quote 
 */
Quote Parser::parse_quote(const JSONObject::ptree & data) const 
{
    Quote quote;

    for (const auto & [key, value] : data) {
        for (const auto & [propertyKey, propertyValue] : value) {
            quote.setQuoteVariable(propertyKey, propertyValue.get_value<std::string>());
        }
    }

    return quote;
}

/**
 * @brief Parse the price history data from the server
 * @author @scawful
 * 
 * @param data 
 * @param ticker 
 * @param freq 
 * @return PriceHistory 
 */
PriceHistory Parser::parse_price_history(const JSONObject::ptree & data, const std::string & ticker, int freq) const 
{
    PriceHistory price_history;
    price_history.setTickerSymbol(ticker);

    for (const auto & [historyKey, historyValue] : data) {
        if (historyKey == "candles") {
            for (const auto& [candleKey, candleValue] : historyValue) {
                tda::Candle newCandle;
                std::string datetime;
                try {
                    newCandle.high = boost::lexical_cast<double>(candleValue.get_child("high.").get_value<std::string>());
                    newCandle.low = boost::lexical_cast<double>(candleValue.get_child("low.").get_value<std::string>());
                    newCandle.open = boost::lexical_cast<double>(candleValue.get_child("open.").get_value<std::string>());
                    newCandle.close = boost::lexical_cast<double>(candleValue.get_child("close.").get_value<std::string>());
                    newCandle.volume = boost::lexical_cast<double>(candleValue.get_child("volume.").get_value<std::string>());
                    std::stringstream dt_ss;
                    std::time_t secsSinceEpoch = boost::lexical_cast<std::time_t>(candleValue.get_child("datetime.").get_value<std::string>());
                    newCandle.raw_datetime = secsSinceEpoch;
                    secsSinceEpoch *= (time_t) 0.001;
                    // %a %d %b %Y - %I:%M:%S%p
                    // %H:%M:%S
                    dt_ss << std::put_time(std::localtime(&secsSinceEpoch), "%a %d %b %Y - %I:%M:%S%p");
                    datetime = dt_ss.str();
                } catch (const boost::wrapexcept<boost::bad_lexical_cast> & e) {
                    std::cout << "parse_price_history:: " << e.what() << std::endl;
                }
                newCandle.datetime = datetime;
                price_history.addCandleByType(newCandle, freq);
                price_history.addCandle(newCandle);
            }
        }
    }
    price_history.setInitialized();
    return price_history;
}

/**
 * @brief Parse the current user principals from the API
 * @author @scawful
 * 
 * @param data 
 * @return UserPrincipals 
 */
UserPrincipals Parser::parse_user_principals(JSONObject::ptree & data) const
{
    UserPrincipals user_principals;

    BOOST_FOREACH (JSONObject::ptree::value_type &v, data.get_child("accounts.")) 
    {
        std::unordered_map<std::string, std::string> account_data;
        for (const auto & [acctKey, acctValue] : v.second)
        {
            account_data[acctKey] = acctValue.get_value<std::string>();
        }
        user_principals.add_account_data(account_data);
    }
    user_principals.set_account_data(user_principals.get_account_data_array(0));

    return user_principals;
}

/**
 * @brief Parse the option chain data from the API
 * @author @scawful
 * 
 * @param data 
 * @return OptionChain 
 */
OptionChain Parser::parse_option_chain(const JSONObject::ptree & data) const
{
    OptionChain optionChain;
    for (const auto & [optionsKey, optionsValue]: data) {
        if (optionsKey == "callExpDateMap") {
            for (const auto & [dateKey, dateValue]: optionsValue)
            {
                OptionsDateTimeObj options_dt_obj;
                options_dt_obj.datetime = dateKey;
                for (const auto & [strikeKey, strikeValue]: dateValue) {
                    StrikePriceMap imported_strike;
                    imported_strike.strikePrice = strikeKey;
                    for (const auto & [contractKey, contractValue]: strikeValue) {
                        for (const auto & [detailsKey, detailsValue]: contractValue) {
                            imported_strike.raw_option[detailsKey] = detailsValue.get_value< std::string >();
                        }
                        options_dt_obj.strikePriceObj.push_back(imported_strike);
                    }
                }
                optionChain.addOptionsDateTimeObj(options_dt_obj);
            }
        }
        
        // -- if -- (optionsKey == "putExpDateMap") -- {
        // --    for -- ( const auto & -- [dateKey, dateValue]: optionsValue ) {
        //     }
        // }

        if ( optionsKey == "underlying" ) {
            for ( const auto & [underlyingKey, underlyingValue]: optionsValue ) {
                optionChain.setUnderlyingVariable(underlyingKey, underlyingValue.get_value<std::string>());
            }
        }

        optionChain.setOptionChainVariable(optionsKey, optionsValue.get_value< std::string >());
    }
    return optionChain;
}

/**
 * @brief Parse the users account data from the API
 * @author @scawful 
 * 
 * @param data 
 * @return Account 
 */
Account Parser::parse_account(const JSONObject::ptree & data) const
{
    Account account;
    for (const auto & [classKey, classValue] : data) {
        for (const auto & [accountKey, accountValue] : classValue) {
            if (accountKey == "positions") {
                for (const auto & [positionListKey, positionListValue] : accountValue) {
                    // positions and balances
                    tda::PositionBalances new_position_balance;
                    for (const auto& [positionsKey, positionsValue] : positionListValue) {
                        new_position_balance.balances[positionsKey] = positionsValue.get_value<std::string>();

                        std::unordered_map<std::string, std::string> pos_field;
                        std::unordered_map<std::string, std::string> instrument;

                        for (const auto & [fieldKey, fieldValue] : positionsValue) {
                            if (fieldKey == "symbol") {
                                new_position_balance.symbol = fieldValue.get_value<std::string>();
                            }

                            pos_field[fieldKey] = fieldValue.get_value<std::string>();
                        }
                        account.add_position(pos_field);
                    }
                    account.add_balance(new_position_balance);
                }
            } else if (accountKey == "currentBalances") {
                for (const auto & [balanceKey, balanceValue] : accountValue) {
                    account.set_balance_variable(balanceKey, balanceValue.get_value<std::string>());
                }
            } else {
                account.set_account_variable(accountKey, accountValue.get_value<std::string>());
            }
        }
    }
    return account;
}

/**
 * @brief Parse account watchlist data from the API
 * @author @scawful
 * 
 * @param data 
 * @return std::vector<tda::Watchlist> 
 */
std::vector<tda::Watchlist> tda::Parser::parse_watchlist_data(const JSONObject::ptree & data) const
{
    std::vector<tda::Watchlist> watchlists;
    // JSON Outer Layer 
    for ( auto const & [dataKey, dataValue] : data ) {
        // Array of Watchlists
        Watchlist watchlist;
        for ( auto const & [watchlistKey, watchlistValue] : dataValue ) {
            // Each element in a watchlist 
            if ( watchlistKey == "name" ) {
                watchlist.setName(watchlistValue.get_value< std::string >());
            } else if ( watchlistKey == "watchlistId" ) {
                watchlist.setId(watchlistValue.get_value< int >());
            } else {
                for (auto const & [elementKey, elementValue] : watchlistValue) { 
                    // Watchlist items 
                    for (auto const & [itemKey, itemValue] : elementValue) { 
                        // Instrument sub array 
                        if ( itemKey == "instrument" ) {
                            std::string symbol;
                            std::string desc;
                            std::string type;
                            for ( auto const & [instrumentKey, instrumentValue] : itemValue ) {
                                if ( instrumentKey == "symbol" ) {
                                    symbol = instrumentValue.get_value< std::string >();
                                } else if ( instrumentKey == "description" ) {
                                    desc = instrumentValue.get_value< std::string >();
                                } else if ( instrumentKey == "assetType" ) {
                                    type = instrumentValue.get_value< std::string >();
                                }
                            }
                            watchlist.addInstrument(symbol, desc, type);
                        } else {
                            watchlist.addVariable(itemKey, itemValue.get_value< std::string >());
                        }
                    }
                }
            }
        }
        watchlists.push_back(watchlist);
    }
    
    return watchlists;
}

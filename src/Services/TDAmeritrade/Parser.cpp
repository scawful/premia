#include "Parser.hpp"
namespace Premia {
using namespace tda;

/**
 * @brief
 *
 * @param data
 * @param chain
 * @param idx
 */
void Parser::parseStrikeMap(const json::ptree &data, OptionChain &chain,
                            int idx) const {
  for (const auto &[dateKey, dateValue] : data) {
    OptionsDateTimeObj options_dt_obj;
    options_dt_obj.datetime = dateKey;
    for (const auto &[strikeKey, strikeValue] : dateValue) {
      StrikePriceMap imported_strike;
      imported_strike.strikePrice = strikeKey;
      for (const auto &[contractKey, contractValue] : strikeValue) {
        for (const auto &[detailsKey, detailsValue] : contractValue) {
          imported_strike.raw_option[detailsKey] =
              detailsValue.get_value<String>();
        }
        options_dt_obj.strikePriceObj.push_back(imported_strike);
      }
    }
    chain.addOptionsDateTimeObj(options_dt_obj);
    chain.addOptionsDateTimeObj(options_dt_obj, idx);
  }
}

/**
 * @brief Take a response from the API as argument and read it into a
 * boost::property_tree
 * @author @scawful
 *
 * @param response
 * @return json::ptree
 */
json::ptree Parser::read_response(CRString response) const {
  std::istringstream json_response(response);
  json::ptree property_tree;
  try {
    read_json(json_response, property_tree);
  } catch (json::ptree_error const &json_parser_error) {
    std::cout << "Parser::read_response: " << json_parser_error.what()
              << std::endl;
  }
  return property_tree;
}

/**
 * @brief Return a human friendly string of the option contract
 *
 * @param symbol
 * @return String
 */
String Parser::parse_option_symbol(CRString symbol) const {
  String underlying;
  String month;
  String day;
  String year;
  String strike;
  String type;

  auto start = symbol.find('_');
  underlying = symbol.substr(0, start);
  month = symbol.substr(start + 1, 2);
  month = months[stoi(month)];
  day = symbol.substr(start + 3, 2);
  year = symbol.substr(start + 5, 2);
  type = symbol.substr(start + 7, 1);
  strike = symbol.substr(start + 8, symbol.size()) + type;

  String date = day + " " + month + " " + year;
  String output = date + "\n" + underlying + " " + strike;

  return output;
}

/**
 * @brief Parse the access token from a response
 * @author @scawful
 *
 * @param response
 * @return String
 */
String Parser::parse_access_token(CRString response) const {
  String access_token;
  json::ptree data = read_response(response);
  for (const auto &[key, value] : data) {
    if (key == "access_token") {
      access_token = value.get_value<String>();
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
Quote Parser::parse_quote(const json::ptree &data) const {
  Quote quote;

  for (const auto &[key, value] : data) {
    for (const auto &[propertyKey, propertyValue] : value) {
      quote.setQuoteVariable(propertyKey, propertyValue.get_value<String>());
    }
  }

  return quote;
}

/**
 * @brief Parse the price history data from the server
 * @author @scawful
 * @note dt format %a %d %b %Y - %I:%M:%S%p,    %H:%M:%S
 *
 * @param data
 * @param ticker
 * @param freq
 * @return PriceHistory
 */
PriceHistory Parser::parse_price_history(const json::ptree &data,
                                         CRString ticker, int freq) const {
  PriceHistory price_history;
  price_history.setTickerSymbol(ticker);

  for (const auto &[historyKey, historyValue] : data) {
    if (historyKey == "candles") {
      for (const auto &[candleKey, candleValue] : historyValue) {
        tda::Candle newCandle;
        String datetime;
        for (const auto &[valueKey, finalValue] : candleValue) {
          try {
            if (valueKey == "open") {
              newCandle.open =
                  boost::lexical_cast<double>(finalValue.get_value<String>());
            } else if (valueKey == "close") {
              newCandle.close =
                  boost::lexical_cast<double>(finalValue.get_value<String>());
            } else if (valueKey == "high") {
              newCandle.high =
                  boost::lexical_cast<double>(finalValue.get_value<String>());
            } else if (valueKey == "low") {
              newCandle.low =
                  boost::lexical_cast<double>(finalValue.get_value<String>());
            } else if (valueKey == "volume") {
              newCandle.volume =
                  boost::lexical_cast<double>(finalValue.get_value<String>());
            } else if (valueKey == "datetime") {
              std::stringstream dt_ss;
              std::time_t secsSinceEpoch = boost::lexical_cast<std::time_t>(
                  finalValue.get_value<String>());
              newCandle.raw_datetime = secsSinceEpoch;
              secsSinceEpoch *= (time_t)0.001;
              dt_ss << std::put_time(std::localtime(&secsSinceEpoch),
                                     "%a %d %b %Y - %I:%M:%S%p");
              datetime = dt_ss.str();
            }
          } catch (const boost::wrapexcept<boost::bad_lexical_cast> &e) {
            std::cout << "parse_price_history:: " << e.what() << std::endl;
          }
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
UserPrincipals Parser::parse_user_principals(json::ptree &data) const {
  UserPrincipals user_principals;

  try {
    BOOST_FOREACH (json::ptree::value_type &v, data.get_child("accounts.")) {
      StringMap account_data;
      for (const auto &[acctKey, acctValue] : v.second) {
        account_data[acctKey] = acctValue.get_value<String>();
      }
      user_principals.add_account_data(account_data);
    }
    user_principals.set_account_data(user_principals.get_account_data_array(0));
  } catch (json::ptree_bad_path) {
    // lazy fix to the account issue
    for (const auto &[key, value] : data) {
      StringMap account_data;
      if (key == "primaryAccountId") {
        account_data[key] = value.get_value<String>();
      }
      user_principals.add_account_data(account_data);
    }
    user_principals.set_account_data(user_principals.get_account_data_array(0));
  }

  return user_principals;
}

/**
 * @brief Parse the option chain data from the API
 * @author @scawful
 *
 * @param data
 * @return OptionChain
 */
OptionChain Parser::parse_option_chain(const json::ptree &data) const {
  OptionChain optionChain;
  for (const auto &[optionsKey, optionsValue] : data) {
    if (optionsKey == "callExpDateMap") {
      parseStrikeMap(optionsValue, optionChain, 1);
    }

    if (optionsKey == "putExpDateMap") {
      parseStrikeMap(optionsValue, optionChain, 0);
    }

    if (optionsKey == "underlying") {
      for (const auto &[underlyingKey, underlyingValue] : optionsValue) {
        optionChain.setUnderlyingVariable(underlyingKey,
                                          underlyingValue.get_value<String>());
      }
    }

    optionChain.setOptionChainVariable(optionsKey,
                                       optionsValue.get_value<String>());
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
Account Parser::parse_account(const json::ptree &data) const {
  Account account;
  for (const auto &[classKey, classValue] : data) {
    for (const auto &[accountKey, accountValue] : classValue) {
      if (accountKey == "positions") {
        for (const auto &[positionListKey, positionListValue] : accountValue) {
          tda::PositionBalances new_position_balance;  // positions and balances
          for (const auto &[positionsKey, positionsValue] : positionListValue) {
            new_position_balance.balances[positionsKey] =
                positionsValue.get_value<String>();
            StringMap pos_field;
            StringMap instrument;
            for (const auto &[fieldKey, fieldValue] : positionsValue) {
              if (fieldKey == "symbol") {
                new_position_balance.symbol = fieldValue.get_value<String>();
              }
              pos_field[fieldKey] = fieldValue.get_value<String>();
            }
            account.add_position(pos_field);
          }
          account.add_balance(new_position_balance);
        }
      } else if (accountKey == "currentBalances") {
        for (const auto &[balanceKey, balanceValue] : accountValue) {
          account.set_balance_variable(balanceKey,
                                       balanceValue.get_value<String>());
        }
      } else {
        account.set_account_variable(accountKey,
                                     accountValue.get_value<String>());
      }
    }
  }
  return account;
}

/**
 * @brief Parse the users account data from the API
 * @author @scawful
 *
 * @param data
 * @return Account
 */
Account Parser::parse_all_accounts(const json::ptree &data) const {
  Account account;
  for (const auto &[key, val] : data) {
    for (const auto &[classKey, classValue] : val) {
      for (const auto &[accountKey, accountValue] : classValue) {
        if (accountKey == "positions") {
          for (const auto &[positionListKey, positionListValue] :
               accountValue) {
            tda::PositionBalances
                new_position_balance;  // positions and balances
            for (const auto &[positionsKey, positionsValue] :
                 positionListValue) {
              std::cout << "Positions List: " << positionListKey << " : "
                        << positionListValue.get_value<String>() << std::endl;
              new_position_balance.balances[positionsKey] =
                  positionsValue.get_value<String>();
              StringMap pos_field;
              StringMap instrument;
              for (const auto &[fieldKey, fieldValue] : positionsValue) {
                if (fieldKey == "symbol") {
                  new_position_balance.symbol = fieldValue.get_value<String>();
                }
                pos_field[fieldKey] = fieldValue.get_value<String>();
              }
              account.add_position(pos_field);
            }
            account.add_balance(new_position_balance);
          }
        } else if (accountKey == "currentBalances") {
          for (const auto &[balanceKey, balanceValue] : accountValue) {
            account.set_balance_variable(balanceKey,
                                         balanceValue.get_value<String>());
          }
        } else {
          account.set_account_variable(accountKey,
                                       accountValue.get_value<String>());
        }
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
 * @return ArrayList<tda::Watchlist>
 */
ArrayList<tda::Watchlist> tda::Parser::parse_watchlist_data(
    const json::ptree &data) const {
  ArrayList<tda::Watchlist> watchlists;
  for (const auto &[dataKey, dataValue] : data) {
    Watchlist watchlist;
    try {
      watchlist.setName(dataValue.get_child("name").get_value<String>());
      watchlist.setId(dataValue.get_child("watchlistId").get_value<int>());
      watchlist.setAccountId(
          dataValue.get_child("accountId").get_value<String>());
    } catch (const json::ptree_error &e) {
      std::cout << e.what() << std::endl;
    }

    for (const auto &[itemKey, itemValue] :
         dataValue.get_child("watchlistItems")) {
      for (const auto &[item2Key, item2Value] : itemValue) {
        if (item2Key == "instrument") {
          try {
            String symbol = item2Value.get_child("symbol").get_value<String>();
            String desc = "";
            String type = item2Value.get_child("assetType").get_value<String>();
            watchlist.addInstrument(symbol, desc, type);
          } catch (const json::ptree_error &e) {
            std::cout << e.what() << std::endl;
          }
        } else {
          watchlist.addVariable(item2Key, itemValue.get_value<String>());
        }
      }
    }
    watchlists.push_back(watchlist);
  }

  return watchlists;
}
}  // namespace Premia
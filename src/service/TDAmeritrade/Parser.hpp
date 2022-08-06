#ifndef Parser_hpp
#define Parser_hpp

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "metatypes.h"
#include "service/TDAmeritrade/Data/Account.hpp"
#include "service/TDAmeritrade/Data/OptionChain.hpp"
#include "service/TDAmeritrade/Data/PriceHistory.hpp"
#include "service/TDAmeritrade/Data/PricingStructures.hpp"
#include "service/TDAmeritrade/Data/Quote.hpp"
#include "service/TDAmeritrade/Data/UserPrincipals.hpp"
#include "service/TDAmeritrade/Data/Watchlist.hpp"

namespace premia {
namespace tda {

namespace json = boost::property_tree;

class Parser {
 private:
  void parseStrikeMap(const json::ptree& data, OptionChain& chain,
                      int idx) const;
  const std::vector<std::string> months = {"N/A", "Jan", "Feb",  "Mar", "Apr",
                                           "May", "Jun", "July", "Aug", "Sept",
                                           "Oct", "Nov", "Dec"};

 public:
  json::ptree read_response(const std::string& response) const;
  std::string parse_option_symbol(const std::string& symbol) const;
  std::string parse_access_token(const std::string& response) const;
  Quote parse_quote(const json::ptree& data) const;
  PriceHistory parse_price_history(const json::ptree& data,
                                   const std::string& ticker, int freq) const;
  UserPrincipals parse_user_principals(json::ptree& data) const;
  OptionChain parse_option_chain(const json::ptree& data) const;
  Account parse_account(const json::ptree& data) const;
  Account parse_all_accounts(const json::ptree& data) const;
  std::vector<Watchlist> parse_watchlist_data(const json::ptree& data) const;
};

}  // namespace tda
}  // namespace premia
#endif
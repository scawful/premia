#ifndef Parser_hpp
#define Parser_hpp

#include "Data/PricingStructures.hpp"
#include "Data/Watchlist.hpp"
#include "Data/Account.hpp"
#include "Data/OptionChain.hpp"
#include "Data/PriceHistory.hpp"
#include "Data/UserPrincipals.hpp"
#include "Data/PriceHistory.hpp"
#include "Data/Quote.hpp"

namespace JSONObject = boost::property_tree;

namespace tda 
{
    class Parser 
    {
    public:
        Parser();

        JSONObject::ptree read_response(const std::string & response) const;
        std::string parse_access_token(const std::string & response) const;
        Quote parse_quote(const JSONObject::ptree & data) const;
        PriceHistory parse_price_history(const JSONObject::ptree & data, const std::string & ticker, int freq) const;
        UserPrincipals parse_user_principals(JSONObject::ptree & data) const;
        OptionChain parse_option_chain(const JSONObject::ptree & data) const;
        Account parse_account(const JSONObject::ptree & data) const;
        std::vector<Watchlist> parse_watchlist_data(const JSONObject::ptree & data) const;
    };

}

#endif
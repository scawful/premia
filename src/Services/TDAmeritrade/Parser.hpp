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
        JSONObject::ptree read_response(CRString response) const;
        String parse_access_token(CRString response) const;
        Quote parse_quote(const JSONObject::ptree & data) const;
        PriceHistory parse_price_history(const JSONObject::ptree & data, CRString ticker, int freq) const;
        UserPrincipals parse_user_principals(JSONObject::ptree & data) const;

        void parseStrikeMap(const JSONObject::ptree & data, OptionChain & chain, int idx) const;
        
        OptionChain parse_option_chain(const JSONObject::ptree & data) const;
        Account parse_account(const JSONObject::ptree & data) const;
        std::vector<Watchlist> parse_watchlist_data(const JSONObject::ptree & data) const;
    };

}

#endif
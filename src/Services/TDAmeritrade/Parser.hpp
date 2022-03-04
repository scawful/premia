#ifndef Parser_hpp
#define Parser_hpp

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
    private:

    public:
        Parser();

        JSONObject::ptree read_response(std::string response);

        Quote parse_quote(JSONObject::ptree data);
        PriceHistory parse_price_history(JSONObject::ptree data);
        UserPrincipals parse_user_principals(JSONObject::ptree data);
        std::vector<Watchlist> parse_watchlist_data(JSONObject::ptree data);
    };

}

#endif
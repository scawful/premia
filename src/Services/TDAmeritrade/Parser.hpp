#ifndef Parser_hpp
#define Parser_hpp

#include "Data/Watchlist.hpp"
#include "Data/UserPrincipals.hpp"
#include "Data/Account.hpp"
#include "Data/OptionChain.hpp"
#include "Data/PriceHistory.hpp"

namespace JSONObject = boost::property_tree;

namespace tda 
{
    class Parser 
    {
    private:

    public:
        Parser();

        JSONObject::ptree read_response(std::string response);

        UserPrincipals parse_user_principals( boost::property_tree::ptree data );
        std::vector<Watchlist> parse_watchlist_data( boost::property_tree::ptree data );        
    };

}

#endif
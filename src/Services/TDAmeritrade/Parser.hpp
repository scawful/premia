#ifndef Parser_hpp
#define Parser_hpp

#include "Metatypes.hpp"
#include "Data/PricingStructures.hpp"
#include "Data/Watchlist.hpp"
#include "Data/Account.hpp"
#include "Data/OptionChain.hpp"
#include "Data/PriceHistory.hpp"
#include "Data/UserPrincipals.hpp"
#include "Data/PriceHistory.hpp"
#include "Data/Quote.hpp"

namespace tda 
{
    class Parser 
    {
    private:
        void parseStrikeMap(const json::ptree & data, OptionChain & chain, int idx) const;
        const StringList months = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec" };

    public:
        json::ptree read_response(CRString response) const;
        String parse_option_symbol(CRString symbol);
        String parse_access_token(CRString response) const;
        Quote parse_quote(const json::ptree & data) const;
        PriceHistory parse_price_history(const json::ptree & data, CRString ticker, int freq) const;
        UserPrincipals parse_user_principals(json::ptree & data) const;
        OptionChain parse_option_chain(const json::ptree & data) const;
        Account parse_account(const json::ptree & data) const;
        Account parse_all_accounts(const json::ptree & data) const;
        ArrayList<Watchlist> parse_watchlist_data(const json::ptree & data) const;
    };

}

#endif
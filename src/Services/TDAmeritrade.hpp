#ifndef TDAmeritrade_hpp
#define TDAmeritrade_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "TDAmeritrade/Client.hpp"
#include "TDAmeritrade/Parser.hpp"
#include "TDAmeritrade/Session.hpp"
#include "TDAmeritrade/Data/Watchlist.hpp"
#include "TDAmeritrade/Data/PricingStructures.hpp"
#include "TDAmeritrade/Data/Account.hpp"
#include "TDAmeritrade/Data/Quote.hpp"
#include "TDAmeritrade/Data/PriceHistory.hpp"
#include "TDAmeritrade/Data/OptionChain.hpp"

namespace tda
{
    namespace JSONObject = boost::property_tree;

    class TDAmeritrade
    {
    private:
        bool session_active;
        std::string _current_ticker;
        std::string _access_token;
        std::string _refresh_token;
        std::string _consumer_key;

        Account current_account;
        Client client;
        Parser parser;

        JSONObject::ptree user_principals;

        // curl functions
        void post_access_token(std::string refresh_token);

        // websocket functions
        JSONObject::ptree get_user_principals();

        // access token
        void request_access_token(bool keep_file);

    public:
        TDAmeritrade();

        std::vector<std::string> get_all_accounts();

        Account getCurrentAccount();
        Account getDefaultAccount();
        std::vector<Watchlist> getWatchlistsByAccount(std::string account_num);
        Quote getQuote(std::string symbol);
        Account getAccount(std::string account_id);
        PriceHistory getPriceHistory(std::string, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext);
        OptionChain getOptionChain(std::string ticker, std::string contractType, std::string strikeCount,
                                   bool includeQuotes, std::string strategy, std::string range,
                                   std::string expMonth, std::string optionType);

        void postOrder(std::string account_id, OrderType order_type, std::string symbol, int quantity);

        void startStreamingSession();

        void fetchAccessToken();
        bool is_session_active();
    };

}

#endif /* TDAmeritrade_hpp */
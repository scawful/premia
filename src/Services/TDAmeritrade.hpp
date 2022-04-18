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
#include "TDAmeritrade/Data/Order.hpp"
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
        void post_access_token() const;

        // websocket functions
        JSONObject::ptree get_user_principals();

        // access token
        void request_access_token(bool keep_file);

    public:
        TDAmeritrade();
        ~TDAmeritrade();

        std::vector<std::string> get_all_accounts();

        Account getCurrentAccount() const;
        Account getDefaultAccount();
        std::vector<Watchlist> getWatchlistsByAccount(const std::string & account_num) const;
        Quote getQuote(const std::string & symbol) const;
        Account getAccount(const std::string & account_id);
        PriceHistory getPriceHistory(const std::string & ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext) const;
        OptionChain getOptionChain(const std::string & ticker, const std::string & contractType, const std::string & strikeCount,
                                   bool includeQuotes, const std::string & strategy, const std::string & range,
                                   const std::string & expMonth, const std::string & optionType) const;

        void postOrder(const std::string & account_id, const Order & order) const;

        void startStreamingSession();

        void fetchAccessToken();
        bool is_session_active();
    };

}

#endif /* TDAmeritrade_hpp */
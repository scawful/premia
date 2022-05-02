#ifndef TDAmeritrade_hpp
#define TDAmeritrade_hpp

#include <curl/curl.h>
#include "Premia.hpp"
#include "TDAmeritrade/Client.hpp"
#include "TDAmeritrade/Parser.hpp"
#include "TDAmeritrade/Socket.hpp"
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
        String _current_ticker;
        String _access_token;
        String _refresh_token;
        String _consumer_key;

        ConsoleLogger consoleLogger;
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

        std::vector<String> get_all_accounts();

        Account getCurrentAccount() const;
        Account getDefaultAccount();
        std::vector<Watchlist> getWatchlistsByAccount(CRString account_num) const;
        Quote getQuote(CRString symbol) const;
        Account getAccount(CRString account_id);
        PriceHistory getPriceHistory(CRString ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext) const;
        OptionChain getOptionChain(CRString ticker, CRString contractType, CRString strikeCount,
                                   bool includeQuotes, CRString strategy, CRString range,
                                   CRString expMonth, CRString optionType) const;

        void postOrder(CRString account_id, const Order & order) const;

        void startStreamingSession();
        void fetchAccessToken();
        bool is_session_active();

        void addLogger(const ConsoleLogger &);
        void addAuth(const String, const String);
    };

}

#endif /* TDAmeritrade_hpp */
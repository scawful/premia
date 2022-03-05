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
    // API Enumerators

    namespace JSONObject = boost::property_tree;

    enum ServiceType
    {
        NONE,
        ADMIN,
        ACTIVES_NASDAQ,
        ACTIVES_NYSE,
        ACTIVES_OTCBB,
        ACTIVES_OPTIONS,
        CHART_EQUITY,
        CHART_FOREX,
        CHART_FUTURES,
        CHART_OPTIONS,
        QUOTE,
        LEVELONE_FUTURES,
        LEVELONE_FOREX,
        LEVELONE_FUTURES_OPTIONS,
        OPTION,
        NEWS_HEADLINE,
        TIMESALE_EQUITY,
        TIMESALE_FUTURES,
        TIMESALE_FOREX,
        TIMESALE_OPTIONS,
        ACCT_ACTIVITY,
        CHART_HISTORY_FUTURES,
        FOREX_BOOK,
        FUTURES_BOOK,
        LISTED_BOOK,
        NASDAQ_BOOK,
        OPTIONS_BOOK,
        FUTURES_OPTIONS_BOOK,
        NEWS_STORY,
        NEWS_HEADLINE_LIST,
        UNKNOWN,
    };

    static const std::string EnumAPIServiceName[]{
        "NONE",
        "ADMIN",
        "ACTIVES_NASDAQ",
        "ACTIVES_NYSE",
        "ACTIVES_OTCBB",
        "ACTIVES_OPTIONS",
        "CHART_EQUITY",
        "CHART_FOREX",
        "CHART_FUTURES",
        "CHART_OPTIONS",
        "QUOTE",
        "LEVELONE_FUTURES",
        "LEVELONE_FOREX",
        "LEVELONE_FUTURES_OPTIONS",
        "OPTION",
        "NEWS_HEADLINE",
        "TIMESALE_EQUITY",
        "TIMESALE_FUTURES",
        "TIMESALE_FOREX",
        "TIMESALE_OPTIONS",
        "ACCT_ACTIVITY",
        "CHART_HISTORY_FUTURES",
        "FOREX_BOOK",
        "FUTURES_BOOK",
        "LISTED_BOOK",
        "NASDAQ_BOOK",
        "OPTIONS_BOOK",
        "FUTURES_OPTIONS_BOOK",
        "NEWS_STORY",
        "NEWS_HEADLINE_LIST",
        "UNKNOWN",
    };

    // API Access Class

    class TDAmeritrade
    {
    private:
        bool _access_token_found;
        bool _session_active;
        bool _user_principals;
        std::string _current_ticker;
        std::string _access_token;
        std::string _refresh_token;
        std::string _consumer_key;
        std::vector<std::thread> _ws_threads;

        Account current_account;
        Client client;
        Parser parser;

        std::shared_ptr<tda::Session> _websocket_session;
        std::shared_ptr<std::vector<std::string>> _websocket_buffer;
        std::vector<std::shared_ptr<std::string const>> _request_queue;

        boost::asio::io_context ioc;
        JSONObject::ptree user_principals;

        // string manipulation
        bool string_replace(std::string &str, const std::string from, const std::string to);

        // curl functions
        void download_file(std::string url, std::string filename);
        void post_access_token(std::string refresh_token);
        void post_account_auth(std::string url, std::string filename);

        // websocket functions
        JSONObject::ptree get_user_principals();
        JSONObject::ptree create_login_request();
        JSONObject::ptree create_logout_request();
        JSONObject::ptree create_service_request(ServiceType serv_type, std::string keys, std::string fields);

        // access token
        void request_access_token(bool keep_file);

    public:
        TDAmeritrade();

        void start_session();
        void start_session(std::string ticker, std::string fields);
        void send_session_request(std::string request);
        void send_logout_request();
        void send_interrupt_signal();
        bool is_session_logged_in();
        std::vector<std::string> get_session_responses();
        std::vector<std::string> get_all_accounts();

        JSONObject::ptree createPropertyTree(std::string ticker, std::string new_url);
        std::vector<Watchlist> retrieveWatchlistsByAccount(std::string account_num);

        bool is_session_active();

        // New Model Functions 
        PriceHistory getPriceHistory(std::string, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext);
        Quote getQuote(std::string symbol);
        Account getAccount(std::string account_id);
        OptionChain getOptionChain(std::string ticker, std::string contractType, std::string strikeCount,
                                   bool includeQuotes, std::string strategy, std::string range,
                                   std::string expMonth, std::string optionType);

        Account getCurrentAccount();
        Account getDefaultAccount();

        void fetchAccessToken();
    };

}

#endif /* TDAmeritrade_hpp */
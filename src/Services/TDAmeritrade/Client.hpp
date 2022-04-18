#ifndef Client_hpp
#define Client_hpp

#include "../../core.hpp"
#include "../../apikey.hpp"
#include "Parser.hpp"
#include "Session.hpp"
#include "Data/UserPrincipals.hpp"
#include "Data/Order.hpp"

namespace JSONObject = boost::property_tree;

namespace tda 
{
    static const std::string EnumAPIValues[]{ "day", "month", "year", "ytd" };
    static const std::string EnumAPIFreq[]{ "minute", "daily", "weekly", "monthly" };
    static const std::string EnumAPIPeriod[]{ "1", "2", "3", "4", "5", "6", "10", "15", "20" };
    static const std::string EnumAPIFreqAmt[]{ "1", "5", "10", "15", "30" };

    enum ServiceType
    {
        NONE,
        ADMIN,
        ACTIVES_NASDAQ, ACTIVES_NYSE, ACTIVES_OTCBB, ACTIVES_OPTIONS,
        CHART_EQUITY, CHART_FOREX, CHART_FUTURES, CHART_OPTIONS,
        QUOTE,
        LEVELONE_FUTURES, LEVELONE_FOREX, LEVELONE_FUTURES_OPTIONS,
        OPTION,
        NEWS_HEADLINE,
        TIMESALE_EQUITY, TIMESALE_FUTURES, TIMESALE_FOREX, TIMESALE_OPTIONS,
        ACCT_ACTIVITY,
        CHART_HISTORY_FUTURES,
        FOREX_BOOK, FUTURES_BOOK, LISTED_BOOK, NASDAQ_BOOK, OPTIONS_BOOK, FUTURES_OPTIONS_BOOK,
        NEWS_STORY, NEWS_HEADLINE_LIST,
        UNKNOWN,
    };

    static const std::string EnumAPIServiceName[]{
        "NONE",
        "ADMIN",
        "ACTIVES_NASDAQ", "ACTIVES_NYSE", "ACTIVES_OTCBB", "ACTIVES_OPTIONS",
        "CHART_EQUITY", "CHART_FOREX", "CHART_FUTURES", "CHART_OPTIONS",
        "QUOTE",
        "LEVELONE_FUTURES", "LEVELONE_FOREX", "LEVELONE_FUTURES_OPTIONS",
        "OPTION",
        "NEWS_HEADLINE",
        "TIMESALE_EQUITY", "TIMESALE_FUTURES", "TIMESALE_FOREX", "TIMESALE_OPTIONS",
        "ACCT_ACTIVITY",
        "CHART_HISTORY_FUTURES",
        "FOREX_BOOK", "FUTURES_BOOK", "LISTED_BOOK", "NASDAQ_BOOK", "OPTIONS_BOOK", "FUTURES_OPTIONS_BOOK",
        "NEWS_STORY", "NEWS_HEADLINE_LIST",
        "UNKNOWN",
    };

    class Client 
    {
    private:
        // Flags
        bool has_access_token = false;
        bool has_user_principals = false;
        bool session_active = false;

        // API Strings 
        std::string api_key;
        std::string refresh_token;
        std::string access_token;

        // API Data 
        Parser parser;
        UserPrincipals user_principals;
        boost::property_tree::ptree _user_principals;

        // WebSocket session variables 
        boost::asio::io_context ioc;
        std::shared_ptr<tda::Session> websocket_session;
        std::shared_ptr<std::vector<std::string>> websocket_buffer;
        std::vector<std::shared_ptr<std::string const>> request_queue;
        std::vector<std::thread> ws_threads;

        // String Manipulation 
        std::string get_api_interval_value(int value) const;
        std::string get_api_frequency_type(int value) const;
        std::string get_api_period_amount(int value) const;
        std::string get_api_frequency_amount(int value) const;
        bool string_replace(std::string & str, const std::string & from, const std::string & to) const;

        // Data Retrieval
        static size_t json_write_callback(const char * contents, size_t size, size_t nmemb, std::string *s);

        // WebSocket functions
        JSONObject::ptree create_login_request();
        JSONObject::ptree create_logout_request();
        JSONObject::ptree create_service_request(ServiceType serv_type, std::string const & keys, std::string const & fields);

        // API Functions 
        void get_user_principals();
        void post_authorized_request(const std::string & endpoint, const std::string & data) const;
        std::string post_account_order(std::string const & account_id) const;
        std::string post_access_token() const;



    public:
        Client();

        std::string send_request(std::string const & endpoint) const;
        std::string send_authorized_request(std::string const & endpoint) const;

        std::string get_watchlist_by_account(std::string const & account_id) const;
        std::string get_price_history(std::string const & symbol, 
                                      PeriodType ptype, int period_amt, 
                                      FrequencyType ftype, int freq_amt, bool ext) const;
        std::string get_option_chain(std::string const & ticker, 
                                     std::string const & contractType, 
                                     std::string const & strikeCount,
                                     bool includeQuotes, 
                                     std::string const & strategy, std::string const & range,
                                     std::string const & expMonth, std::string const & optionType) const;
        std::string get_quote(std::string const & symbol) const;
        std::string get_account(std::string const & account_id);
        std::vector<std::string> get_all_account_ids();

        void post_order(const std::string & account_id, const Order & order) const;

        void start_session();
        void start_session(std::string const & ticker, std::string const & fields);
        void send_session_request(std::string const & request) const;
        void send_logout_request();
        void send_interrupt_signal() const;
        bool is_session_logged_in() const;
        std::vector<std::string> get_session_responses() const;
        std::string get_access_token() const;
        void fetch_access_token();
    };
}

#endif
#ifndef Client_hpp
#define Client_hpp

#include "Utils.hpp"
#include "Parser.hpp"
#include "Socket.hpp"
#include "Metatypes.hpp"
#include "Data/UserPrincipals.hpp"
#include "Data/Order.hpp"

namespace tda 
{
    static const String EnumAPIValues[]{ "day", "month", "year", "ytd" };
    static const String EnumAPIFreq[]{ "minute", "daily", "weekly", "monthly" };
    static const String EnumAPIPeriod[]{ "1", "2", "3", "4", "5", "6", "10", "15", "20" };
    static const String EnumAPIFreqAmt[]{ "1", "5", "10", "15", "30" };

    enum ServiceType {
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

    static const String EnumAPIServiceName[]{
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
    
        bool request_fields[53];
        const char *quote_fields[53] = { "Symbol", "Bid Price", "Ask Price", "Last Price", "Bid Size",
        "Ask Size", "Ask ID", "Bid ID", "Total Volume", "Last Size", "Trade Time", "Quote Time",
        "High Price", "Low Price", "Bid Tick", "Close Price", "Exchange ID", "Marginable", "Shortable",
        "Island Bid", "Island Ask", "Island Volume", "Quote Day", "Trade Day", "Volatility", "Description",
        "Last ID", "Digits", "Open Price", "Net Change", "52 Week High", "52 Week Low", "PE Ratio", "Dividend Amount",
        "Dividend Yield", "Island Bid Size", "Island Ask Size", "NAV", "Fund Price", "Exchange Name", "Dividend Date",
        "Regular Market Quote", "Regular Market Trade", "Regular Market Last Price", "Regular Market Last Size",
        "Regular Market Trade Time", "Regular Market Trade Day", "Regular Market Net Change", "Security Status",
        "Mark", "Quote Time in Long", "Trade Time in Long", "Regular Market Trade Time in Long" };
        
        // Flags
        bool has_access_token = false;
        bool has_user_principals = false;
        bool session_active = false;

        // API Strings 
        String api_key = "";
        String refresh_token  = "";
        String access_token  = "";

        // API Data 
        Parser parser;
        UserPrincipals user_principals;
        json::ptree _user_principals;

        // WebSocket session variables 
        net::io_context ioc;
        std::shared_ptr<tda::Socket> websocket_session;
        std::shared_ptr<ArrayList<String>> websocket_buffer;
        ArrayList<std::shared_ptr<String const>> request_queue;
        ArrayList<std::thread> ws_threads;

        // String Manipulation 
        String get_api_interval_value(int value) const;
        String get_api_frequency_type(int value) const;
        String get_api_period_amount(int value) const;
        String get_api_frequency_amount(int value) const;

        // WebSocket functions
        json::ptree create_login_request();
        json::ptree create_logout_request();
        json::ptree create_service_request(ServiceType serv_type, String const & keys, String const & fields);

        // API Functions 
        void get_user_principals();
        void post_authorized_request(CRString endpoint, CRString data) const;
        String post_account_order(String const & account_id) const;
        String post_access_token() const;

    public:
        Client();
        ~Client();

        String send_request(String const & endpoint) const;
        String send_authorized_request(String const & endpoint) const;

        String get_quote(String const & symbol) const;        
        String get_account(String const & account_id);
        String get_watchlist_by_account(String const & account_id) const;
        String get_price_history(String const & symbol, 
                                 PeriodType ptype, int period_amt, 
                                 FrequencyType ftype, int freq_amt, bool ext) const;
        String get_option_chain(String const & ticker, String const & contractType, 
                                String const & strikeCount, bool includeQuotes, 
                                String const & strategy, String const & range,
                                String const & expMonth, String const & optionType) const;
        ArrayList<String> get_all_account_ids();

        void post_order(CRString account_id, const Order & order) const;

        void start_session();
        void start_session(String const & ticker, String const & fields);
        void send_session_request(String const & request) const;
        void send_logout_request();
        void send_interrupt_signal() const;
        bool is_session_logged_in() const;
        ArrayList<String> get_session_responses() const;
        String get_access_token() const;
        void fetch_access_token();

        void addAuth(CRString, CRString);
    };
}

#endif
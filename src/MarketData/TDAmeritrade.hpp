#ifndef TDAmeritrade_hpp
#define TDAmeritrade_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "TDAmeritrade/Session.hpp"
#include "TDAmeritrade/PricingStructures.hpp"
#include "TDAmeritrade/Account.hpp"
#include "TDAmeritrade/Quote.hpp"
#include "TDAmeritrade/PriceHistory.hpp"
#include "TDAmeritrade/OptionChain.hpp"


namespace tda
{
    // API Enumerators

    enum PeriodType 
    {
        DAY,
        MONTH,
        YEAR,
        YTD
    };

    enum FrequencyType
    {
        MINUTE,
        DAILY,
        WEEKLY,
        MONTHLY
    };

    enum RetrievalType
    {
        GET_QUOTE,
        OPTION_CHAIN,
        PRICE_HISTORY
    };

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

    static const std::string EnumAPIValues[] { "day", "month", "year", "ytd" };
    static const std::string EnumAPIFreq[] { "minute", "daily", "weekly", "monthly" };
    static const std::string EnumAPIPeriod[] { "1", "2", "3", "4", "5", "6", "10", "15", "20" };
    static const std::string EnumAPIFreqAmt[] { "1", "5", "10", "15", "30" };
    static const std::string EnumAPIServiceName[]
    {
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
        int _period;
        int _frequency;
        bool _access_token_found;
        bool _session_active;
        bool _user_principals;
        PeriodType _period_type;
        FrequencyType _frequency_type;
        std::string _base_url;
        std::string _col_name;
        std::string _current_ticker;
        std::string _access_token;
        std::string _refresh_token;
        std::string _consumer_key;
        std::string _access_token_filename;
        std::time_t _access_token_expiration;
        std::vector<std::thread> _ws_threads;

        std::shared_ptr<tda::Session> _websocket_session;
        std::shared_ptr<std::vector<std::string>> _websocket_buffer;
        std::vector<std::shared_ptr<std::string const>> _request_queue;

        boost::asio::io_context ioc;
        boost::property_tree::ptree user_principals;

        // string manipulation 
        std::string get_api_interval_value(int value);
        std::string get_api_frequency_type(int value);
        std::string get_api_period_amount(int value);
        std::string get_api_frequency_amount(int value);
        bool string_replace(std::string& str, const std::string from, const std::string to);

        // curl functions
        void download_file(std::string url, std::string filename);
        void post_access_token( std::string refresh_token, std::string filename );
        void post_account_auth( std::string url, std::string filename );

        // websocket functions
        boost::property_tree::ptree get_user_principals();
        boost::property_tree::ptree create_login_request();
        boost::property_tree::ptree create_logout_request();
        boost::property_tree::ptree create_service_request( ServiceType serv_type, std::string keys, std::string fields );

        // access token 
        void get_access_token( bool keep_file );
        void log_expiration_time();
        bool check_access_token();
        bool check_access_token_expiration();

    public:
        TDAmeritrade( RetrievalType type );

        void start_session();
        void start_session( std::string ticker, std::string fields );
        void send_session_request( std::string request );
        void send_logout_request();
        void send_interrupt_signal();
        bool is_session_logged_in();
        void clear_session_buffer();
        void sync_buffer();
        std::vector<std::string> get_session_responses();
        std::vector<std::string> get_all_accounts();

        boost::property_tree::ptree createPropertyTree( std::string ticker, std::string new_url );
        boost::shared_ptr<tda::Quote> createQuote( std::string ticker );        
        boost::shared_ptr<tda::PriceHistory> createPriceHistory( );
        boost::shared_ptr<tda::PriceHistory> createPriceHistory( std::string ticker );
        boost::shared_ptr<tda::OptionChain> createOptionChain( std::string ticker );
        boost::shared_ptr<tda::Account> createAccount( std::string account_num );

        // setters 
        void set_retrieval_type( RetrievalType type );
        void set_period_type( PeriodType interval );
        void set_col_name( std::string col_name );

        void set_price_history_parameters( std::string ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext = true );
        void set_price_history_parameters( std::string ticker, PeriodType ptype, time_t start_date, time_t end_date, 
                                           FrequencyType ftype, int freq_amt, bool ext = true );
        void set_option_chain_parameters( std::string ticker, std::string contractType, std::string strikeCount, bool includeQuotes, 
                                          std::string strategy, std::string range, std::string expMonth, std::string optionType );


        // auxiliary functions 
        std::string getBaseUrl();
        void manual_authentication();

        bool is_session_active();
    };


}

#endif /* TDAmeritrade_hpp */
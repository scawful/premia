#ifndef TDAmeritrade_hpp
#define TDAmeritrade_hpp

#include "core.hpp"
#include "apikey.hpp"

namespace tda
{
    class Quote 
    {
    private:
        boost::property_tree::ptree quoteData;
        std::map<std::string, std::string> quoteVariables;

        void initVariables();

    public:
        Quote(boost::property_tree::ptree quote_data);

        std::string getQuoteVariable( std::string variable );

    };

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
        QUOTE,
        OPTION_CHAIN,
        PRICE_HISTORY
    };

    static const std::string EnumAPIValues[] { "day", "month", "year", "ytd" };

    class TDAmeritrade 
    {
    private:
        int _period;
        int _frequency;
        PeriodType _period_type;
        FrequencyType _frequency_type;
        std::string _base_url;
        std::string _col_name;

        std::string get_api_interval_value(int value);
        std::string timestamp_from_string(std::string date);
        bool string_replace(std::string& str, const std::string from, const std::string to);
        std::string build_url(std::string ticker, std::string start_date, std::string end_date);
        void download_file(std::string url, std::string filename);

    public:
        TDAmeritrade( RetrievalType type );

        void set_period_type( PeriodType interval );
        void set_col_name( std::string col_name );

        boost::shared_ptr<tda::Quote> createQuote( std::string ticker );
        void retrieveQuoteData( std::string ticker, bool keep_file );
    };


}

#endif /* TDAmeritrade_hpp */
#ifndef TDAmeritrade_hpp
#define TDAmeritrade_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "Data/PricingStructures.hpp"
#include "Data/Quote.hpp"
#include "Data/PriceHistory.hpp"

namespace tda
{
    // Data Objects

    class OptionChain
    {
    private:
        boost::property_tree::ptree optionChainData;

        std::unordered_map<std::string, std::string> callExpDateMap;
        std::unordered_map<std::string, std::string> putExpDateMap;
        std::unordered_map<std::string, std::string> optionChainMap;
        std::vector<OptionsDateTimeObj> optionsDateTimeVector;

        void initVariables();

    public:
        OptionChain( boost::property_tree::ptree option_chain_data );

        std::vector<OptionsDateTimeObj> getOptionsDateTimeObj();
        std::string getCallVariable( std::string variable );
        std::string getPutVariable( std::string variable );
        std::string getOptionChainDataVariable( std::string variable );

    };

    // ================ Account Class Header ================ //

    class Account
    {
    private:
        boost::property_tree::ptree accountData;

        std::unordered_map<std::string, std::string> accountInfoMap;
        std::unordered_map<std::string, std::string> positionsInfoMap;
        
        std::unordered_map<std::string, std::string> currentBalanceMap;        

        void initVariables();

    public:
        Account( boost::property_tree::ptree account_data );

        std::string getAccountVariable( std::string variable );
    };

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
        QUOTE,
        OPTION_CHAIN,
        PRICE_HISTORY
    };

    static const std::string EnumAPIValues[] { "day", "month", "year", "ytd" };
    static const std::string EnumAPIFreq[] { "minute", "daily", "weekly", "monthly" };
    static const std::string EnumAPIPeriod[] { "1", "2", "3", "4", "5", "6", "10", "15", "20" };
    static const std::string EnumAPIFreqAmt[] { "1", "5", "10", "15", "30" };

    // API Access Class

    class TDAmeritrade 
    {
    private:
        int _period;
        int _frequency;
        PeriodType _period_type;
        FrequencyType _frequency_type;
        std::string _base_url;
        std::string _col_name;
        std::string _current_ticker;

        std::string get_api_interval_value(int value);
        std::string get_api_frequency_type(int value);
        std::string get_api_period_amount(int value);
        std::string get_api_frequency_amount(int value);
        std::string timestamp_from_string(std::string date);
        bool string_replace(std::string& str, const std::string from, const std::string to);
        std::string build_url(std::string ticker, std::string start_date, std::string end_date);
        void download_file(std::string url, std::string filename);

    public:
        TDAmeritrade( RetrievalType type );

        void set_retrieval_type( RetrievalType type );
        void set_period_type( PeriodType interval );
        void set_col_name( std::string col_name );

        void set_price_history_parameters( std::string ticker, PeriodType ptype, int period_amt, 
                                           FrequencyType ftype, int freq_amt, bool ext = true );
        void set_price_history_parameters( std::string ticker, PeriodType ptype, 
                                           time_t start_date, time_t end_date,
                                           FrequencyType ftype, int freq_amt, bool ext = true );
        void set_option_chain_parameters( std::string ticker, std::string contractType, std::string strikeCount,
                                          bool includeQuotes, std::string strategy, std::string range,
                                          std::string expMonth, std::string optionType );

        boost::property_tree::ptree createPropertyTree( std::string ticker, std::string new_url );
        boost::shared_ptr<tda::Quote> createQuote( std::string ticker );        
        boost::shared_ptr<tda::PriceHistory> createPriceHistory( );
        boost::shared_ptr<tda::PriceHistory> createPriceHistory( std::string ticker );
        boost::shared_ptr<tda::OptionChain> createOptionChain( std::string ticker );
        boost::shared_ptr<tda::Account> createAccount( std::string account_num );
        void retrieveQuoteData( std::string ticker, bool keep_file );
    };


}

#endif /* TDAmeritrade_hpp */
#include "TDAmeritrade.hpp"

namespace tda
{

    std::string TDAmeritrade::get_api_interval_value(int value)
    {
        return EnumAPIValues[value];
    }

    std::string TDAmeritrade::get_api_frequency_type(int value)
    {
        return EnumAPIFreq[value];
    }

    std::string TDAmeritrade::get_api_period_amount(int value)
    {
        return EnumAPIPeriod[value];
    }

    std::string TDAmeritrade::get_api_frequency_amount(int value)
    {
        return EnumAPIFreqAmt[value];
    }

    std::string TDAmeritrade::timestamp_from_string(std::string date)
    {
        struct std::tm time = {0,0,0,0,0,0,0,0,0};
        std::istringstream ss(date);
        ss >> std::get_time(&time, "%Y-%m-%d");
        if ( ss.fail() )
        {
            std::cerr  << "ERROR: Cannot parse date string (" << date <<"); required format %Y-%m-%d" << std::endl;
            exit(1);
        } 
        time.tm_hour = 0;
        time.tm_min = 0;
        time.tm_sec = 0;
        std::time_t epoch = std::mktime(&time);

        return std::to_string(epoch);
    }

    bool TDAmeritrade::string_replace(std::string& str, const std::string from, const std::string to)
    {
        size_t start = str.find(from);
        if ( start == std::string::npos )
        {
            return false;
        }

        str.replace(start, from.length(), to);
        return true;
    }

    std::string TDAmeritrade::build_url(std::string ticker, std::string start, std::string end)
    {
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);
        string_replace(url, "{start_time}", timestamp_from_string(start));
        string_replace(url, "{end_time}", timestamp_from_string(end));
        string_replace(url, "{periodType}", get_api_interval_value(this->_period_type));
        return url;
    }

    void TDAmeritrade::download_file(std::string url, std::string filename)
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        curl = curl_easy_init();
        if (curl)
        {
            fp = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    void TDAmeritrade::post_access_token( std::string refresh_token, std::string filename )
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        
        curl = curl_easy_init();
        if (curl)
        {
            // set the headers for the request
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // set the url to receive the POST
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.tdameritrade.com/v1/oauth2/token");
                        
            std::string data_post = "grant_type=refresh_token&refresh_token=" + refresh_token + 
                                    "&client_id=" + TDA_API_KEY;

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post);

            // open the file to download access token
            fp = fopen(filename.c_str(), "wb");

            // write data from the url to the function 
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            // run the operations
            res = curl_easy_perform(curl);

            // cleanup yeah yeah 
            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    void TDAmeritrade::post_account_auth( std::string filename )
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        curl = curl_easy_init();
        if (curl)
        {
            struct curl_slist *headers = NULL;
            std::string auth_bearer = "Authorization: Bearer " + _access_token;
            headers = curl_slist_append(headers, auth_bearer.c_str() );
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            
            fp = fopen( filename.c_str(), "wb" );
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform();
        }
    }

    TDAmeritrade::TDAmeritrade( RetrievalType type )
    {
        switch ( type )
        {
            case QUOTE:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
                break;
            case PRICE_HISTORY:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY;
                break;
            case OPTION_CHAIN:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}";
                break;
            default:
                break;
        }

        this->_period_type = DAY;
        this->_frequency_type = MINUTE;
        this->_col_name = "Open";
    }

    void TDAmeritrade::get_access_token()
    {
        std::time_t now = std::time(0);
        std::string access_token_file_name = "access_token_" + std::to_string(now) + ".json";
        post_access_token( _refresh_token, access_token_file_name );
        std::ifstream jsonFile( access_token_file_name, std::ios::in | std::ios::binary );
        boost::property_tree::ptree propertyTree;

        try {
            read_json(jsonFile, propertyTree);
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        jsonFile.close();
        std::remove(access_token_file_name.c_str());

        for ( auto& access_it: propertyTree )
        {
            if ( access_it.first == "access_token" )
            {
                _access_token = access_it.second.get_value<std::string>();
            }
            else if ( access_it.first == "expires_in" )
            {
                now = std::time(0);
                _access_token_expiration = now + access_it.second.get_value<std::time_t>();
            }
        }

    }

    void TDAmeritrade::set_retrieval_type( RetrievalType type )
    {
        switch ( type )
        {
            case QUOTE:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
                break;
            case PRICE_HISTORY:
                //this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY;
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY + "&periodType=ytd&period=1&frequencyType=daily&frequency=1&needExtendedHoursData=true";
                break;
            case OPTION_CHAIN:
                this->_base_url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&includeQuotes={includeQuotes}&strategy={strategy}&range={range}&expMonth={expMonth}&optionType={optionType}";
                break;
            default:
                break;
        }
    }

    void TDAmeritrade::set_period_type( PeriodType periodType )
    {
        this->_period_type = periodType;
    }

    void TDAmeritrade::set_col_name( std::string name )
    {
        this->_col_name = name;
    }

    void TDAmeritrade::set_price_history_parameters( std::string ticker, PeriodType ptype, int period_amt, 
                                                     FrequencyType ftype, int freq_amt, bool ext )
    {
        std::string new_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY + "&periodType={periodType}&period={period}&frequencyType={frequencyType}&frequency={frequency}&needExtendedHoursData={ext}";

        string_replace(new_url, "{ticker}", ticker);
        string_replace(new_url, "{periodType}", get_api_interval_value(ptype));
        string_replace(new_url, "{period}", get_api_period_amount(period_amt));
        string_replace(new_url, "{frequencyType}", get_api_frequency_type(ftype));
        string_replace(new_url, "{frequency}", get_api_frequency_amount(freq_amt));

        if (!ext)
            string_replace(new_url, "{ext}", "false");
        else
            string_replace(new_url, "{ext}", "true");

        this->_base_url = new_url;
        this->_current_ticker = ticker;
    }

    void TDAmeritrade::set_price_history_parameters( std::string ticker, PeriodType ptype, 
                                                     time_t start_date, time_t end_date,
                                                     FrequencyType ftype, int freq_amt, bool ext )
    {
        std::string new_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY + "&periodType={periodType}&period={period}&frequencyType={frequencyType}&frequency={frequency}&needExtendedHoursData={ext}";

        string_replace(new_url, "{ticker}", ticker);
        string_replace(new_url, "{periodType}", get_api_interval_value(ptype));
        string_replace(new_url, "{startDate}", boost::lexical_cast<std::string>(start_date));
        string_replace(new_url, "{endDate}", boost::lexical_cast<std::string>(end_date));
        string_replace(new_url, "{frequencyType}", get_api_frequency_type(ftype));
        string_replace(new_url, "{frequency}", get_api_frequency_amount(freq_amt));

        if (!ext)
            string_replace(new_url, "{ext}", "false");
        else
            string_replace(new_url, "{ext}", "true");

        this->_base_url = new_url;
        this->_current_ticker = ticker;
    }

    void TDAmeritrade::set_option_chain_parameters( std::string ticker, std::string contractType, std::string strikeCount,
                                                    bool includeQuotes, std::string strategy, std::string range,
                                                    std::string expMonth, std::string optionType )
    {
        std::string new_url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&includeQuotes={includeQuotes}&strategy={strategy}&range={range}&expMonth={expMonth}&optionType={optionType}";

        string_replace(new_url, "{ticker}", ticker);
        string_replace(new_url, "{contractType}", contractType);
        string_replace(new_url, "{strikeCount}", strikeCount);
        string_replace(new_url, "{strategy}", strategy);
        string_replace(new_url, "{range}", range);
        string_replace(new_url, "{expMonth}", expMonth);
        string_replace(new_url, "{optionType}", optionType);

        if (!includeQuotes)
            string_replace(new_url, "{includeQuotes}", "FALSE");
        else
            string_replace(new_url, "{includeQuotes}", "TRUE");

        this->_base_url = new_url;
        this->_current_ticker = ticker;
    }

    boost::property_tree::ptree TDAmeritrade::createPropertyTree( std::string ticker, std::string new_url )
    {
        std::time_t now = std::time(0);
        std::string output_file_name = this->_current_ticker + "_" + std::to_string(now) + ".json";
        download_file(new_url, output_file_name);
        std::ifstream jsonFile(output_file_name, std::ios::in | std::ios::binary );
        boost::property_tree::ptree propertyTree;
        
        try {
            read_json(jsonFile, propertyTree);
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        jsonFile.close();
        std::remove(output_file_name.c_str());

        return propertyTree;
    }

    boost::shared_ptr<tda::PriceHistory> TDAmeritrade::createPriceHistory( )
    {
        boost::property_tree::ptree propertyTree = createPropertyTree( this->_current_ticker, this->_base_url );;
        boost::shared_ptr<PriceHistory> new_price_history_data = boost::make_shared<PriceHistory>( propertyTree );

        return new_price_history_data;
    }

    boost::shared_ptr<tda::PriceHistory> TDAmeritrade::createPriceHistory( std::string ticker )
    {
        set_retrieval_type( PRICE_HISTORY );
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);

        boost::property_tree::ptree propertyTree = createPropertyTree( ticker, url );
        boost::shared_ptr<PriceHistory> new_price_history_data = boost::make_shared<PriceHistory>( propertyTree );

        return new_price_history_data;        
    }

    boost::shared_ptr<tda::Quote> TDAmeritrade::createQuote( std::string ticker )
    {
        set_retrieval_type( QUOTE );
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);

        boost::property_tree::ptree propertyTree = createPropertyTree( ticker, url );
        boost::shared_ptr<Quote> new_quote_data = boost::make_shared<Quote>( propertyTree );

        return new_quote_data;
    }

    boost::shared_ptr<tda::OptionChain> TDAmeritrade::createOptionChain( std::string ticker )
    {
        //set_retrieval_type( OPTION_CHAIN );
        boost::property_tree::ptree propertyTree = createPropertyTree( ticker, this->_base_url );
        boost::shared_ptr<OptionChain> new_option_chain_data = boost::make_shared<OptionChain>( propertyTree );
        return new_option_chain_data;
    }

    boost::shared_ptr<tda::Account> TDAmeritrade::createAccount( std::string account_num )
    {
        std::string account_url = "https://api.tdameritrade.com/v1/accounts/{accountNum}?fields=positions,orders";
        string_replace( account_url, "{accountNum}", account_num );

        std::time_t now = std::time(0);
        std::string account_filename = account_num + "_" + std::to_string(now) + ".json";
        post_account_auth( account_filename );

        boost::property_tree::ptree propertyTree;
        boost::shared_ptr<Account> new_account_data = boost::make_shared<Account>( propertyTree );
        return new_account_data;
    }

    std::string TDAmeritrade::getBaseUrl()
    {
        return this->_base_url;
    }

}
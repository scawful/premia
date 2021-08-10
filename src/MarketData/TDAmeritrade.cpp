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

    // SIMPLE CURL DOWNLOAD, NO PARAMETERS OR AUTHORIZATION REQUIRED 
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

    // POSTS REFRESH TOKEN PARAMETERS AND GETS ACCESS TOKEN 
    void TDAmeritrade::post_access_token( std::string refresh_token, std::string filename )
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        res = curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        if (curl)
        {
            // welcome to verbosity 
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // set the url to receive the POST
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.tdameritrade.com/v1/oauth2/token");

            // specify we want to post 
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, TRUE);

            // set the headers for the request
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // chunked request for http1.1/200 ok 
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

            // specify post data, have to url encode the refresh token
            std::string easy_escape = curl_easy_escape(curl, _refresh_token.c_str(), _refresh_token.length());
            std::string data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + TDA_API_KEY;

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

            // open the file to download access token
            fp = fopen(filename.c_str(), "wb");

            // write data from the url to the function 
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

            // run the operations
            res = curl_easy_perform(curl);

            if( res != CURLE_OK )
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            // cleanup yeah yeah 
            curl_easy_cleanup(curl);
            fclose(fp);
        }

        curl_global_cleanup();
    }

    // DOWNLOADS FILE FROM TDA API USING AUTHORIZATION BEARER 
    void TDAmeritrade::post_account_auth( std::string url, std::string filename )
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
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }

    std::string TDAmeritrade::get_user_principals_response()
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        std::string url = "https://api.tdameritrade.com/v1/userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
        std::string filename = "user_principals.json";
        std::string response;

        curl = curl_easy_init();
        if ( curl ) 
        {
            struct curl_slist *headers = NULL;
            std::string auth_bearer = "Authorization: Bearer " + _access_token;
            headers = curl_slist_append(headers, auth_bearer.c_str() );
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            fp = fopen( filename.c_str(), "wb" );
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            res = curl_easy_perform(curl);

            curl_easy_cleanup(curl);
            fclose(fp);
        }

        boost::property_tree::ptree property_tree;
        std::ifstream json_file( filename, std::ios::in | std::ios::binary );

        try {
            read_json( json_file, property_tree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        json_file.close();
        std::remove( filename.c_str());

        for ( auto& principal_it: property_tree )
        {
            if ( principal_it.first == "streamerInfo" )
            {
                for ( auto& streamer_it: principal_it.second )
                {
                    if ( streamer_it.first == "streamerSocketUrl" )
                    {
                        response = streamer_it.second.get_value<std::string>();
                    }
                }
            }
        }

        // userPrincipalsResponse.streamerInfo.streamerSocketUrl
        // var tokenTimeStampAsDateObj = new Date(userPrincipalsResponse.streamerInfo.tokenTimestamp);
        // var tokenTimeStampAsMs = tokenTimeStampAsDateObj.getTime();

        return response;
    }

    // CREATE ACCESS TOKEN FILE 
    void TDAmeritrade::get_access_token( bool keep_file )
    {
        std::time_t now = std::time(0);
        _access_token_filename = "access_token_" + std::to_string(now) + ".json";
        post_access_token( _refresh_token, _access_token_filename );
        
        boost::property_tree::ptree propertyTree;
        std::ifstream jsonFile( _access_token_filename, std::ios::in | std::ios::binary );

        try {
            read_json( jsonFile, propertyTree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        jsonFile.close();
        if ( !keep_file )
            std::remove( _access_token_filename.c_str());

        for ( auto& access_it: propertyTree )
        {
            if ( access_it.first == "access_token" )
            {
                _access_token = access_it.second.get_value<std::string>();
            }
            else if ( access_it.first == "expires_in" )
            {
                _access_token_expiration = std::time(0) + access_it.second.get_value<std::time_t>();
            }
        }

        SDL_Log("Access Token: %s", _access_token.c_str() );
    }

    // SET TIME OF EXPIRATION TO A FILE 
    void TDAmeritrade::log_expiration_time()
    {
        std::string expiration_filename = "exp_" + _access_token_filename;
        std::fstream fp( expiration_filename, std::ios::ate|std::ios::app );

        std::string expiration_time = boost::lexical_cast<std::string>(_access_token_expiration);

        fp << expiration_time;

        fp.close();
    }

    // CHECK EXISTING ACCESS TOKEN FILE AND STORE IF FOUND 
    bool TDAmeritrade::check_access_token()
    {
        for ( const auto& file: std::filesystem::directory_iterator("./") )
        {
            std::string filename = file.path().string();

            std::size_t found_file = filename.find("access_token");

            if ( found_file != std::string::npos )
            {
                boost::property_tree::ptree property_tree;
                std::ifstream json_file( filename, std::ios::in );

                try {
                    read_json( json_file, property_tree );
                }
                catch ( std::exception& json_parser_error ) {
                    SDL_Log("%s", json_parser_error.what() );
                }

                json_file.close();

                for ( auto& access_it: property_tree )
                {
                    if ( access_it.first == "access_token" )
                    {
                        _access_token = access_it.second.get_value<std::string>();
                    }
                }

                if ( _access_token != "nope" )
                    SDL_Log("Valid access token found %s", _access_token.c_str() );
                else
                    return false;

                std::fstream expiration_file( "exp_" + _access_token_filename, std::ios::in );
                while( expiration_file >> _access_token_expiration )
                {
                    SDL_Log("%ld", _access_token_expiration );
                }
                expiration_file.close();

                SDL_Log("Expiration %ld", _access_token_expiration);

                return true;
            }
        }
        SDL_Log("Valid access token not found.");

        return false;
    }

    // VERIFY WHETHER THE CURRENT ACCESS TOKEN HAS EXPIRED 
    bool TDAmeritrade::check_access_token_expiration()
    {
        std::time_t now = std::time(0);

        if ( now > _access_token_expiration )
        {
            // token has expired.
            SDL_Log("Access token has expired! Expiration at %ld", _access_token_expiration );
            return true;
        }

        std::time_t time_left = now - _access_token_expiration;

        // token has not expired.
        SDL_Log("Access token has not yet expired. T - %ld remains.", time_left );
        return false;
    }

    TDAmeritrade::TDAmeritrade( RetrievalType type )
    {
        switch ( type )
        {
            case QUOTE:
                _base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
                break;
            case PRICE_HISTORY:
                _base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY;
                break;
            case OPTION_CHAIN:
                _base_url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}";
                break;
            default:
                break;
        }

        _period_type = DAY;
        _frequency_type = MINUTE;
        _col_name = "Open";
        _refresh_token = REFRESH_TOKEN;
        _access_token = "nope";

        if ( !check_access_token() )
        {
            get_access_token( true );
            log_expiration_time();
            SDL_Log("Expiration: %ld", _access_token_expiration );
        }

        check_access_token_expiration();
    }

    void TDAmeritrade::start_session()
    {
        std::string host = "wss://" + get_user_principals_response() + "/ws";
        std::string port = "80";
        std::string text;
        boost::asio::io_context ioc;
        boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};
        //load_root_certificates( context );
        std::make_shared<tda::Session>(ioc, context)->run( host.c_str(), port.c_str(), text.c_str() );

        ioc.run();
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

        
        if ( now > _access_token_expiration )
        {
            SDL_Log("Get Access Token Triggered");
            //get_access_token( true );
        }

        post_account_auth( account_url, account_filename );
        std::ifstream jsonFile( account_filename, std::ios::in | std::ios::binary );

        boost::property_tree::ptree propertyTree;
        
        try {
            read_json( jsonFile, propertyTree );
        }
        catch ( std::exception& json_parser_error ) {
            SDL_Log("%s", json_parser_error.what() );
        }

        jsonFile.close();
        std::remove(account_filename.c_str());

        boost::shared_ptr<Account> new_account_data = boost::make_shared<Account>( propertyTree );
        return new_account_data;
    }

    std::string TDAmeritrade::getBaseUrl()
    {
        return this->_base_url;
    }

    void TDAmeritrade::manual_authentication()
    {
        get_access_token( true );
        log_expiration_time();
    }

}
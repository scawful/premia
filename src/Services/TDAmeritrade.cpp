#include "TDAmeritrade.hpp"

namespace tda
{
    /**
     * @brief Post the refresh token to the API service and retrieve an access token
     * @author @scawful
     *
     * @param refresh_token
     * @param filename
     */
    void TDAmeritrade::post_access_token() const
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        res = curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        if (curl)
        {
            // set the url to receive the POST
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.tdameritrade.com/v1/oauth2/token");

            // specify we want to post
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

            // set the headers for the request
            struct curl_slist *headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // chunked request for http1.1/200 ok
            struct curl_slist *chunk = nullptr;
            chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

            // specify post data, have to url encode the refresh token
            std::string easy_escape = curl_easy_escape(curl, _refresh_token.c_str(), _refresh_token.length());
            std::string data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + _consumer_key;

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

            // open the file to download access token
            fp = fopen("access_token.json", "wb");

            // write data from the url to the function
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            // specify the user agent
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

            // run the operations
            res = curl_easy_perform(curl);

            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

            // cleanup yeah yeah
            curl_easy_cleanup(curl);
            fclose(fp);
        }

        curl_global_cleanup();
    }

    /**
     * @brief authenticated user principal fields
     * @author @scawful
     *
     * @return JSONObject::ptree
     */
    JSONObject::ptree TDAmeritrade::get_user_principals()
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        std::string url = "https://api.tdameritrade.com/v1/userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
        std::string filename = "user_principals.json";
        std::string response;

        curl = curl_easy_init();
        struct curl_slist *headers = nullptr;
        std::string auth_bearer = "Authorization: Bearer " + _access_token;
        headers = curl_slist_append(headers, auth_bearer.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        fp = fopen(filename.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        fclose(fp);

        JSONObject::ptree property_tree;
        std::ifstream json_file(filename, std::ios::in | std::ios::binary);

        try {
            read_json(json_file, property_tree);
        } catch (const std::exception & json_parser_error) {
            std::cout << json_parser_error.what() << std::endl;
        }

        json_file.close();
        user_principals = property_tree;
        std::remove(filename.c_str());

        return property_tree;
    }

    /**
     * @brief Retrieve the access token from the API service
     * @author @scawful
     *
     * @param keep_file
     */
    void TDAmeritrade::request_access_token(bool keep_file)
    {
        std::time_t now = std::time(0);
        post_access_token();

        JSONObject::ptree propertyTree;
        std::ifstream jsonFile("access_token.json", std::ios::in | std::ios::binary);

        try {
            read_json(jsonFile, propertyTree);
        } catch (const JSONObject::ptree_error & json_parser_error) {
            std::cout << json_parser_error.what() << std::endl;
        }

        jsonFile.close();
        if (!keep_file)
            std::remove("access_token.json");

        for (const auto & [key, value] : propertyTree) {
            if (key == "access_token") {
                _access_token = value.get_value<std::string>();
            }
        }
    }

    // PUBLIC FUNCTIONS =========================================================================

    /**
     * @brief TDAmeritrade::TDAmeritrade object constructor and destructor
     * @author @scawful
     * 
     * @param type 
     */
    TDAmeritrade::TDAmeritrade() : session_active(false) { }
    TDAmeritrade::~TDAmeritrade()=default;
    
    /**
     * @brief Retrieve account information from user principals and return as a list
     * @author @scawful
     * 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> TDAmeritrade::get_all_accounts()
    {
        std::vector<std::string> accounts;
        client.fetch_access_token();
        this->_access_token = client.get_access_token();
        get_user_principals();

        for (const auto & [key, value] : user_principals) {
            if (key == "accounts") {
                for (const auto & [key2, val2] : value) {
                    for (const auto & [elementKey, elementValue] : val2) {
                        if (elementKey == "accountId") {
                            accounts.push_back(elementValue.get_value<std::string>());
                        }
                    }
                }
            }
        }

        return accounts;
    }

    /**
     * @brief Get the current Account object saved in the interface
     * @author @scawful
     * 
     * @return Account 
     */
    Account TDAmeritrade::getCurrentAccount() const
    {
        return current_account;
    }

    /**
     * @brief Get the default account associated with the API key
     * @author @scawful
     * 
     * @return Account 
     */
    Account TDAmeritrade::getDefaultAccount()
    {
        std::string account_id = client.get_all_account_ids().at(0);
        return getAccount(account_id);
    }

    /**
     * @brief Retrieve a vector of Watchlists under the account id 
     * 
     * @param account_num 
     * @return std::vector<Watchlist> 
     */
    std::vector<Watchlist> TDAmeritrade::getWatchlistsByAccount(const std::string & account_num) const
    {
        std::string response = client.get_watchlist_by_account(account_num);
        return parser.parse_watchlist_data(parser.read_response(response));
    }

    /**
     * @brief Create Quote object by ticker using client and parser 
     * @author @scawful
     *
     * @param symbol
     * @return FundOwnership
     */
    Quote TDAmeritrade::getQuote(const std::string & symbol) const
    {
        std::string response = client.get_quote(symbol);
        return parser.parse_quote(parser.read_response(response));
    }

    /**
     * @brief Create Account object by account id using client and parser 
     * @author @scawful
     * 
     * @param account_id 
     * @return Account 
     */
    Account TDAmeritrade::getAccount(const std::string & account_id)
    {
        std::string response = client.get_account(account_id);
        this->current_account = parser.parse_account(parser.read_response(response));
        return current_account;
    }

    /**
     * @brief Create a tda::OptionChain object from API service with ticker parameter
     * @author @scawful
     * 
     * @param ticker 
     * @return tda::OptionChain 
     */
    OptionChain TDAmeritrade::getOptionChain(const std::string & ticker, const std::string & contractType, const std::string & strikeCount,
                                            bool includeQuotes, const std::string & strategy, const std::string & range,
                                            const std::string & expMonth, const std::string & optionType) const
    {
        std::string response = client.get_option_chain(ticker, contractType, strikeCount, includeQuotes, strategy, range, expMonth, optionType);
        return parser.parse_option_chain(parser.read_response(response));
    }

    /**
     * @brief Retrieve PriceHistory by parameters using client and parser  
     * 
     * @param ticker 
     * @param mode 
     * @param time 
     * @return PriceHistory 
     */
    PriceHistory TDAmeritrade::getPriceHistory(const std::string & ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext) const
    {
        std::string response = client.get_price_history(ticker, ptype, period_amt, ftype, freq_amt, ext);
        return parser.parse_price_history(parser.read_response(response), ticker, ftype);
    }


    /**
     * @brief Interface for Client post_order routine
     * @todo potentially make it specify the parameters beforehand 
     * 
     * @param account_id 
     * @param order_type 
     * @param symbol 
     * @param quantity 
     */
    void TDAmeritrade::postOrder(const std::string & account_id, const tda::Order & order) const
    {
        client.post_order(account_id, order);
    }

    /**
     * @brief Substitute for old streaming session start up routine
     * @author @scawful
     * 
     */
    void TDAmeritrade::startStreamingSession()
    {
        client.start_session();
    }

    void TDAmeritrade::fetchAccessToken()
    {
        client.fetch_access_token();
    }

    bool TDAmeritrade::is_session_active()
    {
        return session_active;
    }

    void TDAmeritrade::addLogger(const Premia::ConsoleLogger & logger)
    {
        this->consoleLogger = logger;
    }

    void TDAmeritrade::addAuth(const std::string key, const std::string token)
    {
        this->_consumer_key = key;
        this->_refresh_token = token;
        client.addAuth(_consumer_key, _refresh_token);  
    }

}
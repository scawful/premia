#include "TDAmeritrade.hpp"

namespace tda
{
    /**
     * @brief Replace a substring within a string with given parameter
     * @author @scawful
     * 
     * @param str 
     * @param from 
     * @param to 
     * @return true 
     * @return false 
     */
    bool TDAmeritrade::string_replace(std::string &str, const std::string from, const std::string to)
    {
        size_t start = str.find(from);
        if (start == std::string::npos)
            return false;

        str.replace(start, from.length(), to);
        return true;
    }

    /**
     * @brief Download data from API service without parameters or authorization
     * @author @scawful
     *
     * @param url
     * @param filename
     */
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

    /**
     * @brief Post the refresh token to the API service and retrieve an access token
     * @author @scawful
     *
     * @param refresh_token
     * @param filename
     */
    void TDAmeritrade::post_access_token(std::string refresh_token)
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        res = curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        if (curl)
        {
            // welcome to verbosity
            // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            // set the url to receive the POST
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.tdameritrade.com/v1/oauth2/token");

            // specify we want to post
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

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
     * @brief Retrieve file from API service using access token for authorization
     * @author @scawful
     *
     * @param url
     * @param filename
     */
    void TDAmeritrade::post_account_auth(std::string url, std::string filename)
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;

        curl = curl_easy_init();
        if (curl)
        {
            struct curl_slist *headers = NULL;
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
        }
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
        if (curl)
        {
            struct curl_slist *headers = NULL;
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
        }

        JSONObject::ptree property_tree;
        std::ifstream json_file(filename, std::ios::in | std::ios::binary);

        try
        {
            read_json(json_file, property_tree);
        }
        catch (std::exception &json_parser_error)
        {
            SDL_Log("%s", json_parser_error.what());
        }

        json_file.close();
        _user_principals = true;
        user_principals = property_tree;
        std::remove(filename.c_str());

        return property_tree;
    }

    /**
     * @brief creates ptree with login request for websocket session
     * @author @scawful
     *
     * @return JSONObject::ptree
     */
    JSONObject::ptree TDAmeritrade::create_login_request()
    {
        JSONObject::ptree user_principals, credentials, requests, parameters;
        user_principals = get_user_principals();

        std::unordered_map<std::string, std::string> account_data;
        BOOST_FOREACH (JSONObject::ptree::value_type &v, user_principals.get_child("accounts."))
        {
            for (auto &acct_it : v.second)
            {
                account_data[acct_it.first] = acct_it.second.get_value<std::string>();
            }
            break;
        }

        requests.put("service", "ADMIN");
        requests.put("requestid", 1);
        requests.put("command", "LOGIN");
        requests.put("account", account_data["accountId"]);
        requests.put("source", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

        // format credentials
        // "userid": userPrincipalsResponse.accounts[0].accountId,
        // "token": userPrincipalsResponse.streamerInfo.token,
        // "company": userPrincipalsResponse.accounts[0].company,
        // "segment": userPrincipalsResponse.accounts[0].segment,
        // "cddomain": userPrincipalsResponse.accounts[0].accountCdDomainId,
        // "usergroup": userPrincipalsResponse.streamerInfo.userGroup,
        // "accesslevel": userPrincipalsResponse.streamerInfo.accessLevel,
        // "authorized": "Y",
        // "timestamp": tokenTimeStampAsMs,
        // "appid": userPrincipalsResponse.streamerInfo.appId,
        // "acl": userPrincipalsResponse.streamerInfo.acl

        credentials.put("company", account_data["company"]);
        credentials.put("segment", account_data["segment"]);
        credentials.put("cddomain", account_data["accountCdDomainId"]);
        credentials.put("userid", account_data["accountId"]);
        credentials.put("usergroup", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.userGroup")));
        credentials.put("accesslevel", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.accessLevel")));
        credentials.put("authorized", "Y");
        credentials.put("acl", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.acl")));

        // token timestamp format :: 2021-08-10T14:57:11+0000
        std::tm token_timestamp = {};
        std::string original_token_timestamp = user_principals.get<std::string>("streamerInfo.tokenTimestamp");

        // remove 'T' character
        std::size_t found = original_token_timestamp.find('T');
        std::string reformatted_token_timestamp = original_token_timestamp.replace(found, 1, " ");

        // remove the UTC +0000 portion, will adjust for this manually
        found = reformatted_token_timestamp.find('+');
        reformatted_token_timestamp = reformatted_token_timestamp.replace(found, 5, " ");
        SDL_Log("Reformatted Token Timestamp: %s", reformatted_token_timestamp.c_str());

        // convert string timestamp into time_t
        std::istringstream ss(reformatted_token_timestamp);
        ss >> std::get_time(&token_timestamp, "%Y-%m-%d %H:%M:%S");
        if (ss.fail())
        {
            SDL_Log("Token timestamp parse failed!");
        }
        else
        {
            // this is disgusting i'm sorry
            std::time_t token_timestamp_as_sec = std::mktime(&token_timestamp);
            std::chrono::time_point token_timestamp_point = std::chrono::system_clock::from_time_t(token_timestamp_as_sec);
            auto duration = token_timestamp_point.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            millis -= 18000000;
            credentials.put("timestamp", millis);
        }

        credentials.put("appid", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

        // format parameters
        std::string credential_str;
        for (auto &cred_it : credentials)
        {
            credential_str += cred_it.first + "%3D" + cred_it.second.get_value<std::string>() + "%26";
        }
        std::size_t end = credential_str.size();
        credential_str.replace(end - 3, 3, "");

        parameters.put("token", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.token")));
        parameters.put("version", "1.0");
        parameters.put("credential", credential_str);

        // include in requests
        requests.add_child("parameters", parameters);

        // "requests": [
        //     {
        //         "service": "ADMIN",
        //         "command": "LOGIN",
        //         "requestid": 0,
        //         "account": userPrincipalsResponse.accounts[0].accountId,
        //         "source": userPrincipalsResponse.streamerInfo.appId,
        //         "parameters": {
        //             "credential": jsonToQueryString(credentials),
        //             "token": userPrincipalsResponse.streamerInfo.token,
        //             "version": "1.0"
        //         }
        //     }
        // ]

        return requests;
    }

    /**
     * @brief create ptree of logout request for websocket session
     * @author @scawful
     *
     * @return JSONObject::ptree
     */
    JSONObject::ptree TDAmeritrade::create_logout_request()
    {
        JSONObject::ptree user_principals, credentials, requests, parameters;
        user_principals = get_user_principals();

        std::unordered_map<std::string, std::string> account_data;
        BOOST_FOREACH (JSONObject::ptree::value_type &v, user_principals.get_child("accounts."))
        {
            for (auto &acct_it : v.second)
            {
                account_data[acct_it.first] = acct_it.second.get_value<std::string>();
            }
            break;
        }

        requests.put("service", "ADMIN");
        requests.put("requestid", 1);
        requests.put("command", "LOGOUT");
        requests.put("account", account_data["accountId"]);
        requests.put("source", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

        requests.add_child("parameters", parameters);

        return requests;
    }

    /**
     * @brief Create generic service request with given keys and fields
     * @author @scawful
     *
     * @param serv_type
     * @param keys
     * @param fields
     * @return JSONObject::ptree
     */
    JSONObject::ptree TDAmeritrade::create_service_request(ServiceType serv_type, std::string keys, std::string fields)
    {
        JSONObject::ptree requests, parameters, user_principals;
        user_principals = get_user_principals();

        // gets first account by default, maybe change later
        std::unordered_map<std::string, std::string> account_data;
        BOOST_FOREACH (JSONObject::ptree::value_type &v, user_principals.get_child("accounts."))
        {
            for (auto &acct_it : v.second)
            {
                account_data[acct_it.first] = acct_it.second.get_value<std::string>();
            }
            break;
        }

        requests.put("service", EnumAPIServiceName[serv_type]);
        requests.put("requestid", 1);
        requests.put("command", "SUBS");
        requests.put("account", account_data["accountId"]);
        requests.put("source", user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

        parameters.put("keys", keys);
        parameters.put("fields", fields);

        requests.add_child("parameters", parameters);

        return requests;
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
        post_access_token(_refresh_token);

        JSONObject::ptree propertyTree;
        std::ifstream jsonFile("access_token.json", std::ios::in | std::ios::binary);

        try
        {
            read_json(jsonFile, propertyTree);
        }
        catch (std::exception &json_parser_error)
        {
            SDL_Log("%s", json_parser_error.what());
        }

        jsonFile.close();
        if (!keep_file)
            std::remove("access_token.json");

        for (auto &access_it : propertyTree)
        {
            if (access_it.first == "access_token")
            {
                _access_token = access_it.second.get_value<std::string>();
            }
        }
    }

    // PUBLIC FUNCTIONS =========================================================================

    /**
     * @brief Construct a new TDAmeritrade::TDAmeritrade object
     * @author @scawful
     * 
     * @todo Decouple from retrieval type initialization 
     * 
     * @param type 
     */
    TDAmeritrade::TDAmeritrade()
    {
        // case GET_QUOTE:
        //     _base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
        // case PRICE_HISTORY:
        //     _base_url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY;
        // case OPTION_CHAIN:
        //     _base_url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}";
        _session_active = false;
        _user_principals = false;
        _col_name = "Open";
        _refresh_token = REFRESH_TOKEN;
        _access_token = "nope";

        request_access_token(false);
    }

    /**
     * @brief Start a WebSocket session
     * @author @scawful
     * 
     */
    void TDAmeritrade::start_session()
    {
        std::string host;
        std::string port = "443";
        try
        {
            host = get_user_principals().get<std::string>("streamerInfo.streamerSocketUrl");
        }
        catch (std::exception &ptree_bad_path)
        {
            SDL_Log("Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: %s", ptree_bad_path.what());
        }

        JSONObject::ptree login_request = create_login_request();
        JSONObject::ptree logout_request = create_logout_request();

        // for testing
        std::ifstream requests_json("requests_test.json", std::ios::out);
        JSONObject::write_json("requests_test.json", login_request);

        std::stringstream login_text_stream;
        JSONObject::write_json(login_text_stream, login_request);
        std::string login_text = login_text_stream.str();

        std::stringstream requests_text_stream;
        JSONObject::write_json(requests_text_stream, create_service_request(QUOTE, "TLT", "0,1,2,3,4,5,6,7,8"));
        std::string request_text = requests_text_stream.str();

        std::stringstream logout_text_stream;
        JSONObject::write_json(logout_text_stream, logout_request);
        std::string logout_text = logout_text_stream.str();

        std::stringstream chart_request_stream;
        JSONObject::write_json(chart_request_stream, create_service_request(CHART_EQUITY, "AAPL", "0,1,2,3,4,5,6,7,8"));
        std::string chart_equity_text = chart_request_stream.str();

        _request_queue.push_back(std::make_shared<std::string const>(login_text));
        _request_queue.push_back(std::make_shared<std::string const>(request_text));
        _request_queue.push_back(std::make_shared<std::string const>(logout_text));

        boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};

        _websocket_session = std::make_shared<tda::Session>(ioc, context, _request_queue);
        _websocket_session->run(host.c_str(), port.c_str());

        SDL_Log("~~~");
        _session_active = true;

        std::thread session_thread(boost::bind(&boost::asio::io_context::run, &ioc));
        session_thread.detach();

        //_websocket_session->send_message( std::make_shared<std::string const>(chart_equity_text) );
    }

    /**
     * @brief Start a WebSocket session quickly with a ticker and fields
     * @author @scawful
     * 
     * @param ticker 
     * @param fields 
     */
    void TDAmeritrade::start_session(std::string ticker, std::string fields)
    {
        std::string host;
        std::string port = "443";
        try
        {
            host = get_user_principals().get<std::string>("streamerInfo.streamerSocketUrl");
        }
        catch (std::exception &ptree_bad_path)
        {
            SDL_Log("Start_Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: %s", ptree_bad_path.what());
        }

        pt::ptree login_request = create_login_request();

        std::stringstream login_text_stream;
        JSONObject::write_json(login_text_stream, login_request);
        std::string login_text = login_text_stream.str();

        std::stringstream requests_text_stream;
        JSONObject::write_json(requests_text_stream, create_service_request(QUOTE, ticker, fields));
        std::string request_text = requests_text_stream.str();

        _request_queue.push_back(std::make_shared<std::string const>(login_text));
        _request_queue.push_back(std::make_shared<std::string const>(request_text));

        boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};

        _websocket_session = std::make_shared<tda::Session>(ioc, context, _request_queue);
        _websocket_session->run(host.c_str(), port.c_str());

        _session_active = true;

        std::thread session_thread(boost::bind(&boost::asio::io_context::run, &ioc));
        session_thread.detach();
    }

    /**
     * @brief Send a message to the current WebSocket session
     * @author @scawful
     * 
     * @param request 
     */
    void TDAmeritrade::send_session_request(std::string request)
    {
        _websocket_session->send_message(std::make_shared<std::string const>(request));
    }

    /**
     * @brief Send a logout request to the current WebSocket session
     * @author @scawful
     * 
     */
    void TDAmeritrade::send_logout_request()
    {
        _websocket_session->interrupt();
        pt::ptree logout_request = create_logout_request();
        std::stringstream logout_text_stream;
        pt::write_json(logout_text_stream, logout_request);
        std::string logout_text = logout_text_stream.str();
        _websocket_session->send_message(std::make_shared<std::string const>(logout_text));
    }

    /**
     * @brief Send an interuppt signal to the current WebSocket session
     * @author @scawful
     * 
     */
    void TDAmeritrade::send_interrupt_signal()
    {
        _websocket_session->interrupt();
    }

    /**
     * @brief Check if user is logged into the current WebSocket session
     * @author @scawful
     * 
     * @return true 
     * @return false 
     */
    bool TDAmeritrade::is_session_logged_in()
    {
        if (_session_active)
            return _websocket_session->is_logged_in();
        else
            return false;
    }

    /**
     * @brief Get a list of all the responses logged in the WebSocket session
     * @author @scawful
     * 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> TDAmeritrade::get_session_responses()
    {
        return _websocket_session->receive_response();
    }

    /**
     * @brief Retrieve account information from user principals and return as a list
     * @author @scawful
     * 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> TDAmeritrade::get_all_accounts()
    {
        std::vector<std::string> accounts;
        if (!_user_principals) {
            get_user_principals();
        }

        for (auto &array : user_principals.get_child("accounts")) {
            for (auto &each_element : array.second) {
                if (each_element.first == "accountId") {
                    accounts.push_back(each_element.second.get_value<std::string>());
                }
            }
        }

        return accounts;
    }

    /**
     * @brief Creat a JSONObject that holds json information downloaded from a file 
     * @author @scawful
     * 
     * @param ticker 
     * @param new_url 
     * @return JSONObject::ptree 
     */
    JSONObject::ptree TDAmeritrade::createPropertyTree(std::string ticker, std::string new_url)
    {
        std::time_t now = std::time(0);
        std::string output_file_name = this->_current_ticker + "_" + std::to_string(now) + ".json";
        download_file(new_url, output_file_name);
        std::ifstream jsonFile(output_file_name, std::ios::in | std::ios::binary);
        JSONObject::ptree propertyTree;

        try
        {
            read_json(jsonFile, propertyTree);
        }
        catch (std::exception &json_parser_error)
        {
            SDL_Log("%s", json_parser_error.what());
        }

        jsonFile.close();
        std::remove(output_file_name.c_str());

        return propertyTree;
    }

    /**
     * @brief Create a tda::OptionChain object from API service with ticker parameter
     * @author @scawful
     * 
     * @param ticker 
     * @return boost::shared_ptr<tda::OptionChain> 
     */
    OptionChain TDAmeritrade::createOptionChain(std::string ticker, std::string contractType, std::string strikeCount,
                                                                        bool includeQuotes, std::string strategy, std::string range,
                                                                        std::string expMonth, std::string optionType)
    {
        std::string url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + TDA_API_KEY + "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&includeQuotes={includeQuotes}&strategy={strategy}&range={range}&expMonth={expMonth}&optionType={optionType}";

        string_replace(url, "{ticker}", ticker);
        string_replace(url, "{contractType}", contractType);
        string_replace(url, "{strikeCount}", strikeCount);
        string_replace(url, "{strategy}", strategy);
        string_replace(url, "{range}", range);
        string_replace(url, "{expMonth}", expMonth);
        string_replace(url, "{optionType}", optionType);

        if (!includeQuotes)
            string_replace(url, "{includeQuotes}", "FALSE");
        else
            string_replace(url, "{includeQuotes}", "TRUE");

        JSONObject::ptree propertyTree = createPropertyTree(ticker, url);
        return OptionChain(propertyTree);
    }

    std::vector<Watchlist> TDAmeritrade::retrieveWatchlistsByAccount(std::string account_num)
    {
        std::string url = "https://api.tdameritrade.com/v1/accounts/{accountNum}/watchlists";
        string_replace(url, "{accountNum}", account_num);
        std::time_t now = std::time(0);
        std::string account_filename = account_num + "_" + std::to_string(now) + ".json";

        post_account_auth(url, account_filename);
        std::ifstream jsonFile(account_filename, std::ios::in | std::ios::binary);

        JSONObject::ptree propertyTree;
        try
        {
            read_json(jsonFile, propertyTree);
        }
        catch (std::exception &json_parser_error)
        {
            SDL_Log("%s", json_parser_error.what());
        }

        jsonFile.close();
        std::remove(account_filename.c_str());
        return parser.parse_watchlist_data(propertyTree);
    }

    bool TDAmeritrade::is_session_active()
    {
        return _session_active;
    }

    /**
     * @brief 
     * 
     * @param ticker 
     * @param mode 
     * @param time 
     * @return PriceHistory 
     */
    PriceHistory TDAmeritrade::getPriceHistory(std::string ticker, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext)
    {
        std::string response = client.get_price_history(ticker, ptype, period_amt, ftype, freq_amt, ext);
        return parser.parse_price_history(parser.read_response(response));
    }

    /**
     * @brief Retrieve Quote object by ticker using client and parser 
     *
     * @param symbol
     * @return FundOwnership
     */
    Quote TDAmeritrade::getQuote(std::string symbol)
    {
        std::string response = client.get_quote(symbol);
        return parser.parse_quote(parser.read_response(response));
    }
    /**
    * 
    * @brief Retrieve Account data by account ID number 
    * 
    * 
    */
    Account TDAmeritrade::getAccount(std::string account_id)
    {
        std::string response = client.get_account(account_id);
        this->current_account = parser.parse_account(parser.read_response(response));
        return current_account;
    }

    OptionChain TDAmeritrade::getOptionChain(std::string ticker, std::string contractType, std::string strikeCount,
        bool includeQuotes, std::string strategy, std::string range,
        std::string expMonth, std::string optionType)
    {

    }

    Account TDAmeritrade::getCurrentAccount()
    {
        return current_account;
    }

    void TDAmeritrade::fetchAccessToken()
    {
        client.fetch_access_token();
    }
}
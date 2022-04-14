#include "Client.hpp"

using namespace tda;

std::string Client::get_api_interval_value(int value) const
{
    return EnumAPIValues[value];
}

std::string Client::get_api_frequency_type(int value) const
{
    return EnumAPIFreq[value];
}

std::string Client::get_api_period_amount(int value) const
{
    return EnumAPIPeriod[value];
}

std::string Client::get_api_frequency_amount(int value) const
{
    return EnumAPIFreqAmt[value];
}

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
bool Client::string_replace(std::string& str, const std::string from, const std::string to) const
{
    size_t start = str.find(from);
    if (start == std::string::npos)
        return false;

    str.replace(start, from.length(), to);
    return true;
}

/**
 * @brief Get JSON data from server and store into string 
 * @author @scawful
 * 
 * @param contents 
 * @param size 
 * @param nmemb 
 * @param s 
 * @return size_t 
 */
size_t Client::json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t new_length = size * nmemb;
    try {
        s->append((char*)contents, new_length);
    } catch(std::bad_alloc &e) {
        // handle memory problem
        return 0;
    }
    return new_length;
}

/**
 * @brief creates ptree with login request for websocket session
 * @author @scawful
 * @todo refactor the _user_principals line with the UserPrincipals class 
 *
 * @return JSONObject::ptree
 */
JSONObject::ptree Client::create_login_request()
{
    JSONObject::ptree credentials;
    JSONObject::ptree requests;
    JSONObject::ptree parameters;

    std::unordered_map<std::string, std::string> account_data;
    BOOST_FOREACH (JSONObject::ptree::value_type &v, _user_principals.get_child("accounts."))
    {
        for (const auto& [key, value] : (JSONObject::ptree) v.second)
        {
            account_data[key] = value.get_value<std::string>();
        }
        break;
    }

    requests.put("service", "ADMIN");
    requests.put("requestid", 1);
    requests.put("command", "LOGIN");
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

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
    credentials.put("usergroup", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.userGroup")));
    credentials.put("accesslevel", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.accessLevel")));
    credentials.put("authorized", "Y");
    credentials.put("acl", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.acl")));

    // token timestamp format :: 2021-08-10T14:57:11+0000
    std::tm token_timestamp = {};
    std::string original_token_timestamp = _user_principals.get<std::string>("streamerInfo.tokenTimestamp");

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

    credentials.put("appid", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

    // format parameters
    std::string credential_str;
    for (const auto& [key, value] : (JSONObject::ptree) credentials)
    {
        credential_str += key + "%3D" + value.get_value<std::string>() + "%26";
    }
    std::size_t end = credential_str.size();
    credential_str.replace(end - 3, 3, "");

    parameters.put("token", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.token")));
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
JSONObject::ptree Client::create_logout_request()
{
    JSONObject::ptree credentials, requests, parameters;

    std::unordered_map<std::string, std::string> account_data;
    BOOST_FOREACH (JSONObject::ptree::value_type &v, _user_principals.get_child("accounts."))
    {
        for (const auto & [key,value] : (JSONObject::ptree) v.second)
        {
            account_data[key] = value.get_value<std::string>();
        }
        break;
    }

    requests.put("service", "ADMIN");
    requests.put("requestid", 1);
    requests.put("command", "LOGOUT");
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

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
JSONObject::ptree Client::create_service_request(ServiceType serv_type, std::string const & keys, std::string const & fields)
{
    JSONObject::ptree requests;
    JSONObject::ptree parameters;

    // gets first account by default, maybe change later
    std::unordered_map<std::string, std::string> account_data;
    BOOST_FOREACH (JSONObject::ptree::value_type &v, _user_principals.get_child("accounts."))
    {
        for (const auto & [key,value] : (JSONObject::ptree) v.second)
        {
            account_data[key] = value.get_value<std::string>();
        }
        break;
    }

    requests.put("service", EnumAPIServiceName[serv_type]);
    requests.put("requestid", 1);
    requests.put("command", "SUBS");
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<std::string>(JSONObject::ptree::path_type("streamerInfo.appId")));

    parameters.put("keys", keys);
    parameters.put("fields", fields);

    requests.add_child("parameters", parameters);

    return requests;
}

/**
 * @brief Get User Principals from API endpoint
 *        Parse and store in UserPrincipals object for local use
 * @author @scawful
 * 
 */
void Client::get_user_principals() 
{
    std::string endpoint = "https://api.tdameritrade.com/v1/userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
    std::string response = send_authorized_request(endpoint);
    JSONObject::ptree json_principals = parser.read_response(response);
    _user_principals = parser.read_response(response);
    user_principals = parser.parse_user_principals(json_principals);
    has_user_principals = true;
}

/**
 * @brief @brief Send a POST request for placing an order 
 * 
 * @return std::string 
 */
std::string Client::post_account_order(std::string const &account_id)
{
    CURL *curl;
    CURLcode res;
    std::string response;
    std::string endpoint = "https://api.tdameritrade.com/v1/accounts/{account_id}/orders";
    string_replace(endpoint, "{account_id}", account_id);

    // @todo implement a callback for curl_global_init code response 
    res = curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        // set the url to receive the POST
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

        // specify we want to post
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

        // set the headers for the request
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string auth_bearer = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_bearer.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // chunked request for http1.1/200 ok
        struct curl_slist *chunk = nullptr;
        chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        // specify post data, have to url encode the refresh token
        // @todo set up order request 
        std::string easy_escape = curl_easy_escape(curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
        std::string data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + TDA_API_KEY;

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

        // write data from the url to the function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // specify the user agent
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

        // run the operations
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // cleanup yeah yeah
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return response;
}

/**
 * @brief Send a POST request using the consumer key and refresh token to get the access token
 * @author @scawful
 * 
 * @return std::string 
 */
std::string Client::post_access_token() const
{
    CURL *curl;
    CURLcode res;
    std::string response;

    // @todo implement a callback for curl_global_init code response 
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
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // chunked request for http1.1/200 ok
        struct curl_slist *chunk = nullptr;
        chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        // specify post data, have to url encode the refresh token
        std::string easy_escape = curl_easy_escape(curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
        std::string data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + TDA_API_KEY;

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

        // write data from the url to the function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // specify the user agent
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

        // run the operations
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // cleanup yeah yeah
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return response;
}

/**
 * @brief Construct a new Client:: Client object
 * @author @scawful
 * 
 */
Client::Client()
{
    this->refresh_token = REFRESH_TOKEN;
}

/**
 * @brief Send a request for data from the API using the json callback
 * 
 * @param endpoint 
 * @return std::string 
 */
std::string Client::send_request(std::string const & endpoint) const 
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return response;
}

/**
 * @brief Send an authorized request for data from the API using the json callback
 * @author @scawful
 * 
 * @param endpoint 
 * @return std::string 
 */
std::string Client::send_authorized_request(std::string const & endpoint) const
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        std::string auth_bearer = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_bearer.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
    }

    return response;
}

/**
 * @brief Prepare a request for watchlist data by an account number
 *        Return the API response
 * @author @scawful
 * 
 * @param account_id 
 * @return std::string 
 */
std::string Client::get_watchlist_by_account(std::string const &account_id)
{
    std::string url = "https://api.tdameritrade.com/v1/accounts/{accountNum}/watchlists";
    string_replace(url, "{accountNum}", account_id);
    return send_authorized_request(url);
}

/**
 * @brief Prepare a request from the API for price history information 
 *        Return the API response
 * @author @scawful
 * 
 * @param symbol 
 * @param ptype 
 * @param period_amt 
 * @param ftype 
 * @param freq_amt 
 * @param ext 
 * @return std::string 
 */
std::string Client::get_price_history(std::string const &symbol, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext)
{
    std::string url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + TDA_API_KEY + "&periodType={periodType}&period={period}&frequencyType={frequencyType}&frequency={frequency}&needExtendedHoursData={ext}";

    string_replace(url, "{ticker}", symbol);
    string_replace(url, "{periodType}", get_api_interval_value(ptype));
    string_replace(url, "{period}", get_api_period_amount(period_amt));
    string_replace(url, "{frequencyType}", get_api_frequency_type(ftype));
    string_replace(url, "{frequency}", get_api_frequency_amount(freq_amt));

    if (!ext)
        string_replace(url, "{ext}", "false");
    else
        string_replace(url, "{ext}", "true");

    return send_request(url);
}

/**
 * @brief Prepare a request from the API for option chain data 
 *        Return the API response
 * @author @scawful
 * 
 * @param ticker 
 * @param contractType 
 * @param strikeCount 
 * @param includeQuotes 
 * @param strategy 
 * @param range 
 * @param expMonth 
 * @param optionType 
 * @return std::string 
 */
std::string Client::get_option_chain(std::string const &ticker, std::string const & contractType, std::string const & strikeCount,
                                     bool includeQuotes, std::string const & strategy, std::string const & range,
                                     std::string const & expMonth, std::string const &  optionType)
{
    OptionChain option_chain;
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

    return send_request(url);
}

/**
 * @brief Request quote data by the instrument symbol
 *        Return the API response
 * @author @scawful
 * 
 * @param symbol 
 * @return std::string 
 */
std::string Client::get_quote(std::string const & symbol)
{
    std::string url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
    string_replace(url, "{ticker}", symbol);
    return send_request(url);
}

/**
 * @brief Request account data by the account id
 *        Return the API response after authorization
 * @author @scawful
 * 
 * @param account_id 
 * @return std::string 
 */
std::string Client::get_account(std::string const & account_id)
{
    get_user_principals();
    std::string account_url = "https://api.tdameritrade.com/v1/accounts/{accountNum}?fields=positions,orders";
    string_replace(account_url, "{accountNum}", account_id);
    return send_authorized_request(account_url);
}

/**
 * @brief Create a vector of all the account ids present on the API key 
 * @author @scawful
 * 
 * @todo make this less smelly 
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string> Client::get_all_account_ids()
{
    std::vector<std::string> accounts;
    fetch_access_token();
    get_user_principals();

    for (const auto & [key, value] : _user_principals.get_child("accounts")) {
        for (const auto & [acctKey, acctValue] : value) {
            if (key == "accountId") {
                accounts.push_back(acctValue.get_value<std::string>());
            }
        }
    }

    return accounts;
}

/**
 * @brief 
 * 
 * @param account_id 
 * @param order_type 
 * @param symbol 
 * @param quantity 
 */
void Client::post_order(std::string const & account_id, OrderType order_type, std::string const & symbol, int quantity)
{
    
}

/**
 * @brief Start a WebSocket session
 * @todo implement this 
 * 
 */
void Client::start_session() 
{
    std::string host;
    std::string port = "443";
    try
    {
        host = _user_principals.get<std::string>("streamerInfo.streamerSocketUrl");
    }
    catch (JSONObject::ptree_error const & ptree_bad_path)
    {
        SDL_Log("Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: %s", ptree_bad_path.what());
    }

    JSONObject::ptree login_request = create_login_request();
    JSONObject::ptree logout_request = create_logout_request();

    // for testing
    std::ifstream requests_json("requests_test.json", std::ios::out);
    JSONObject::write_json("requests_test.json", login_request);

    std::stringstream login_text_stream;
    write_json(login_text_stream, login_request);
    std::string login_text = login_text_stream.str();

    std::stringstream requests_text_stream;
    write_json(requests_text_stream, create_service_request(QUOTE, "TLT", "0,1,2,3,4,5,6,7,8"));
    std::string request_text = requests_text_stream.str();

    std::stringstream logout_text_stream;
    JSONObject::write_json(logout_text_stream, logout_request);
    std::string logout_text = logout_text_stream.str();

    std::stringstream chart_request_stream;
    write_json(chart_request_stream, create_service_request(CHART_EQUITY, "AAPL", "0,1,2,3,4,5,6,7,8"));
    std::string chart_equity_text = chart_request_stream.str();

    request_queue.push_back(std::make_shared<std::string const>(login_text));
    request_queue.push_back(std::make_shared<std::string const>(request_text));
    request_queue.push_back(std::make_shared<std::string const>(logout_text));

    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};

    websocket_session = std::make_shared<tda::Session>(ioc, context, request_queue);
    websocket_session->run(host.c_str(), port.c_str());

    SDL_Log("~~~");
    session_active = true;

    std::thread session_thread(boost::bind(&boost::asio::io_context::run, &ioc));
    session_thread.detach();

    // send an initial message to get price data from AAPL
    websocket_session->send_message( std::make_shared<std::string const>(chart_equity_text) );
}

/**
 * @brief Start a WebSocket session quickly with a ticker and fields
 * @author @scawful
 * 
 * @param ticker 
 * @param fields 
 */
void Client::start_session(std::string const & ticker, std::string const & fields)
{
    std::string host;
    std::string port = "443";
    try {
        host = _user_principals.get<std::string>("streamerInfo.streamerSocketUrl");
    } catch (JSONObject::ptree_error const & ptree_bad_path) {
        SDL_Log("Start_Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: %s", ptree_bad_path.what());
    }

    pt::ptree login_request = create_login_request();

    std::stringstream login_text_stream;
    write_json(login_text_stream, login_request);
    std::string login_text = login_text_stream.str();

    std::stringstream requests_text_stream;
    write_json(requests_text_stream, create_service_request(QUOTE, ticker, fields));
    std::string request_text = requests_text_stream.str();

    request_queue.push_back(std::make_shared<std::string const>(login_text));
    request_queue.push_back(std::make_shared<std::string const>(request_text));

    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};

    websocket_session = std::make_shared<tda::Session>(ioc, context, request_queue);
    websocket_session->run(host.c_str(), port.c_str());

    session_active = true;

    std::thread session_thread(boost::bind(&boost::asio::io_context::run, &ioc));
    session_thread.detach();
}

/**
 * @brief Send a request to the current WebSocket session
 * @author @scawful
 * 
 * @param request 
 */
void Client::send_session_request(std::string const & request) const
{
    websocket_session->send_message(std::make_shared<std::string const>(request));
}

/**
 * @brief Send a logout request to the current WebSocket session
 * @author @scawful
 * 
 */
void Client::send_logout_request()
{
    websocket_session->interrupt();
    pt::ptree logout_request = create_logout_request();
    std::stringstream logout_text_stream;
    pt::write_json(logout_text_stream, logout_request);
    std::string logout_text = logout_text_stream.str();
    websocket_session->send_message(std::make_shared<std::string const>(logout_text));
}

/**
 * @brief Send an interrupt signal to the current WebSocket session
 * @author @scawful 
 * 
 */
void Client::send_interrupt_signal() const
{
    websocket_session->interrupt();
}

/**
 * @brief Check if user is logged into the current WebSocket session
 * @author @scawful
 * 
 * @return true 
 * @return false 
 */
bool Client::is_session_logged_in() const
{
    if (session_active)
        return websocket_session->is_logged_in();
    else
        return false;
}

/**
 * @brief Get a list of all the responses logged in the WebSocket session
 * @author @scawful
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string> Client::get_session_responses() const
{
    return websocket_session->receive_response();
}

/**
 * @brief Getter for API access token
 * 
 * @return std::string 
 */
std::string Client::get_access_token() const
{
    return access_token;
}

/**
 * @brief Public retrieval of access token
 * @author @scawful
 * 
 */
void Client::fetch_access_token()
{
    access_token = parser.parse_access_token(post_access_token());
    has_access_token = true;
}

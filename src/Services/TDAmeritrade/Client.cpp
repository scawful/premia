#include "Client.hpp"

using namespace tda;

String Client::get_api_interval_value(int value) const { return EnumAPIValues[value]; }
String Client::get_api_frequency_type(int value) const { return EnumAPIFreq[value]; }
String Client::get_api_period_amount(int value) const { return EnumAPIPeriod[value]; }
String Client::get_api_frequency_amount(int value) const { return EnumAPIFreqAmt[value]; }

/**
 * @brief creates ptree with login request for websocket session
 * @author @scawful
 * @todo refactor the _user_principals line with the UserPrincipals class 
 *
 * @return json::ptree
 */
json::ptree 
Client::create_login_request() {
    json::ptree credentials;
    json::ptree requests;
    json::ptree parameters;

    StringMap account_data;
    BOOST_FOREACH (json::ptree::value_type &v, _user_principals.get_child("accounts.")) {
        for (auto &acct_it : v.second)
        {
            account_data[acct_it.first] = acct_it.second.get_value<std::string>();
        }
        break;
    }

    requests.put("service", "ADMIN");
    requests.put("command", "LOGIN");
    requests.put("requestid", 1);
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<std::string>(json::ptree::path_type("streamerInfo.appId")));
    
    credentials.put("userid", account_data["accountId"]);
    credentials.put("company", account_data["company"]);
    credentials.put("segment", account_data["segment"]);
    credentials.put("cddomain", account_data["accountCdDomainId"]);
    credentials.put("userid", account_data["accountId"]);
    credentials.put("usergroup", _user_principals.get<String>(json::ptree::path_type("streamerInfo.userGroup")));
    credentials.put("accesslevel", _user_principals.get<String>(json::ptree::path_type("streamerInfo.accessLevel")));
    credentials.put("authorized", "Y");
    credentials.put("acl", _user_principals.get<String>(json::ptree::path_type("streamerInfo.acl")));

    
    std::tm token_timestamp = {}; // token timestamp format :: 2021-08-10T14:57:11+0000
    std::string original_token_timestamp = _user_principals.get<String>("streamerInfo.tokenTimestamp");

    // remove 'T' character
    std::size_t found = original_token_timestamp.find('T');
    std::string reformatted_token_timestamp = original_token_timestamp.replace(found, 1, " ");

    // remove the UTC +0000 portion, will adjust for this manually
    found = reformatted_token_timestamp.find('+');
    reformatted_token_timestamp = reformatted_token_timestamp.replace(found, 5, " ");

    // convert string timestamp into time_t
    std::istringstream ss(reformatted_token_timestamp);
    ss >> std::get_time(&token_timestamp, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        SDL_Log("Token timestamp parse failed!");
    } else {
        // this is disgusting i'm sorry
        std::time_t token_timestamp_as_sec = std::mktime(&token_timestamp);
        std::chrono::time_point token_timestamp_point = std::chrono::system_clock::from_time_t(token_timestamp_as_sec);
        auto duration = token_timestamp_point.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        millis -= 18000000;
        millis -= 3600000;
        credentials.put("timestamp", millis);
    }

    credentials.put("appid", _user_principals.get<String>(json::ptree::path_type("streamerInfo.appId")));

    String credential_str; // format parameters
    for (const auto& [key, value] : (json::ptree) credentials) {
        credential_str += key + "%3D" + value.get_value<String>() + "%26";
    }
    std::size_t end = credential_str.size();
    credential_str.replace(end - 3, 3, "");

    parameters.put("token", _user_principals.get<String>(json::ptree::path_type("streamerInfo.token")));
    parameters.put("version", "1.0");
    parameters.put("credential", credential_str);

    requests.add_child("parameters", parameters); // include in requests
    return requests;
}

/**
 * @brief create ptree of logout request for websocket session
 * @author @scawful
 *
 * @return json::ptree
 */
json::ptree 
Client::create_logout_request()
{
    json::ptree credentials;
    json::ptree requests;
    json::ptree parameters;

    StringMap account_data;
    BOOST_FOREACH (json::ptree::value_type &v, _user_principals.get_child("accounts.")) {
        for (const auto & [key,value] : (json::ptree) v.second) {
            account_data[key] = value.get_value<String>();
        }
        break;
    }

    requests.put("service", "ADMIN");
    requests.put("requestid", 1);
    requests.put("command", "LOGOUT");
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<String>(json::ptree::path_type("streamerInfo.appId")));
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
 * @return json::ptree
 */
json::ptree 
Client::create_service_request(ServiceType serv_type, String const & keys, String const & fields)
{
    json::ptree requests;
    json::ptree parameters;

    StringMap account_data;
    BOOST_FOREACH (json::ptree::value_type &v, _user_principals.get_child("accounts.")) {
        for (const auto & [key,value] : (json::ptree) v.second) {
            account_data[key] = value.get_value<String>();
        }
        break;
    }

    requests.put("service", EnumAPIServiceName[serv_type]);
    requests.put("requestid", 2);
    requests.put("command", "SUBS");
    requests.put("account", account_data["accountId"]);
    requests.put("source", _user_principals.get<String>(json::ptree::path_type("streamerInfo.appId")));

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
    String endpoint = "https://api.tdameritrade.com/v1/userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
    String response = send_authorized_request(endpoint);
    json::ptree json_principals = parser.read_response(response);
    _user_principals = parser.read_response(response);
    user_principals = parser.parse_user_principals(json_principals);
    has_user_principals = true;
}

/**
 * @brief POST Request 
 * 
 * @param endpoint 
 * @param data 
 */
void 
Client::post_authorized_request(CRString endpoint, CRString data) const 
{
    CURL *curl;
    String response;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

    struct curl_slist *headers = nullptr;
    curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    String auth_bearer = "Authorization: Bearer " + access_token;
    curl_slist_append(headers, auth_bearer.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // specify post data, have to url encode the refresh token
    String easy_escape = curl_easy_escape(curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
    String data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + api_key;

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

    // write data from the url to the function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlbacks::json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // specify the user agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

    // run the operations
    curl_easy_perform(curl);

    // cleanup yeah yeah
    curl_easy_cleanup(curl);
}

/**
 * @brief Send a POST request for placing an order 
 * 
 * @return String 
 */
String 
Client::post_account_order(String const &account_id) const
{
    CURL *curl;
    CURLcode res;
    String response;
    String endpoint = "https://api.tdameritrade.com/v1/accounts/{account_id}/orders";
    Utils::string_replace(endpoint, "{account_id}", account_id);

    curl = curl_easy_init();
    // set the url to receive the POST
    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

    // specify we want to post
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, true);

    // set the headers for the request
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    String auth_bearer = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_bearer.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // chunked request for http1.1/200 ok
    struct curl_slist *chunk = nullptr;
    chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    // specify post data, have to url encode the refresh token
    // @todo set up order request 
    String easy_escape = curl_easy_escape(curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
    String data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + api_key;

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

    // write data from the url to the function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlbacks::json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // specify the user agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

    // run the operations
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        throw Premia::TDAClientException("post_account_order() failed", curl_easy_strerror(res));

    curl_easy_cleanup(curl);
    return response;
}

/**
 * @brief Send a POST request using the consumer key and refresh token to get the access token
 * @author @scawful
 * 
 * @return String 
 */
String Client::post_access_token() const
{
    CURL *curl;
    CURLcode res;
    String response;

    curl = curl_easy_init();
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
    String easy_escape = curl_easy_escape(curl, refresh_token.c_str(), static_cast<int>(refresh_token.length()));
    String data_post = "grant_type=refresh_token&refresh_token=" + easy_escape + "&client_id=" + api_key;

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_post.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_post.length());

    // write data from the url to the function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlbacks::json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // specify the user agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "premia-agent/1.0");

    // run the operations
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        throw Premia::TDAClientException("post_access_token() failed", curl_easy_strerror(res));

    curl_easy_cleanup(curl);
    return response;
}

Client::Client() { curl_global_init(CURL_GLOBAL_SSL); }
Client::~Client() { curl_global_cleanup(); }

/**
 * @brief Send a request for data from the API using the json callback
 * 
 * @param endpoint 
 * @return String 
 */
String 
Client::send_request(String const & endpoint) const 
{
    CURL *curl;
    CURLcode res;
    String response;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlbacks::json_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        throw Premia::TDAClientException("send_request() failed", curl_easy_strerror(res));
    
    curl_easy_cleanup(curl);
    return response;
}

/**
 * @brief Send an authorized request for data from the API using the json callback
 * @author @scawful
 * 
 * @param endpoint 
 * @return String 
 */
String 
Client::send_authorized_request(String const & endpoint) const
{
    CURL *curl;
    CURLcode res;
    String response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        String auth_bearer = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_bearer.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlbacks::json_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) 
            throw Premia::TDAClientException("send_authorized_request() failed", curl_easy_strerror(res));

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
 * @return String 
 */
String 
Client::get_watchlist_by_account(const String  & account_id) const
{
    String url = "https://api.tdameritrade.com/v1/accounts/{accountNum}/watchlists";
    Utils::string_replace(url, "{accountNum}", account_id);
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
 * @return String 
 */
String 
Client::get_price_history(CRString symbol, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext) const
{
    String url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/pricehistory?apikey=" + api_key + "&periodType={periodType}&period={period}&frequencyType={frequencyType}&frequency={frequency}&needExtendedHoursData={ext}";

    Utils::string_replace(url, "{ticker}", symbol);
    Utils::string_replace(url, "{periodType}", get_api_interval_value(ptype));
    Utils::string_replace(url, "{period}", get_api_period_amount(period_amt));
    Utils::string_replace(url, "{frequencyType}", get_api_frequency_type(ftype));
    Utils::string_replace(url, "{frequency}", get_api_frequency_amount(freq_amt));

    if (!ext)
        Utils::string_replace(url, "{ext}", "false");
    else
        Utils::string_replace(url, "{ext}", "true");

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
 * @return String 
 */
String 
Client::get_option_chain(String const &ticker, String const & contractType, String const & strikeCount,
                         bool includeQuotes, String const & strategy, String const & range,
                         String const & expMonth, String const &  optionType) const
{
    OptionChain option_chain;
    String url = "https://api.tdameritrade.com/v1/marketdata/chains?apikey=" + api_key + "&symbol={ticker}&contractType={contractType}&strikeCount={strikeCount}&includeQuotes={includeQuotes}&strategy={strategy}&range={range}&expMonth={expMonth}&optionType={optionType}";

    Utils::string_replace(url, "{ticker}", ticker);
    Utils::string_replace(url, "{contractType}", contractType);
    Utils::string_replace(url, "{strikeCount}", strikeCount);
    Utils::string_replace(url, "{strategy}", strategy);
    Utils::string_replace(url, "{range}", range);
    Utils::string_replace(url, "{expMonth}", expMonth);
    Utils::string_replace(url, "{optionType}", optionType);

    if (!includeQuotes)
        Utils::string_replace(url, "{includeQuotes}", "FALSE");
    else
        Utils::string_replace(url, "{includeQuotes}", "TRUE");

    return send_request(url);
}

/**
 * @brief Request quote data by the instrument symbol
 *        Return the API response
 * @author @scawful
 * 
 * @param symbol 
 * @return String 
 */
String 
Client::get_quote(String const & symbol) const
{
    String url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + api_key;
    Utils::string_replace(url, "{ticker}", symbol);
    return send_request(url);
}

/**
 * @brief Request account data by the account id
 *        Return the API response after authorization
 * @author @scawful
 * 
 * @param account_id 
 * @return String 
 */
String 
Client::get_account(String const & account_id)
{
    get_user_principals();
    String account_url = "https://api.tdameritrade.com/v1/accounts/{accountNum}?fields=positions,orders";
    Utils::string_replace(account_url, "{accountNum}", account_id);
    return send_authorized_request(account_url);
}

String 
Client::get_all_accounts()
{
    get_user_principals();
    String account_url = "https://api.tdameritrade.com/v1/accounts/?fields=positions,orders";
    return send_authorized_request(account_url);
}

/**
 * @brief Create a vector of all the account ids present on the API key 
 * @author @scawful
 * 
 * @todo make this less smelly 
 * 
 * @return ArrayList<String> 
 */
ArrayList<String> 
Client::get_all_account_ids()
{
    ArrayList<String> accounts;
    fetch_access_token();
    get_user_principals();
    
    for (const auto & [key, value] : _user_principals) {
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
 * @brief 
 * 
 * @param account_id 
 * @param order_type 
 * @param symbol 
 * @param quantity 
 */
void 
Client::post_order(CRString account_id, const tda::Order & order) const
{
    String endpoint = "https://api.tdameritrade.com/v1/accounts/{accountId}/orders";
    Utils::string_replace(endpoint, "{accountId}", account_id);
    post_authorized_request(endpoint, order.getString());
}

/**
 * @brief Start a WebSocket session
 * @todo implement this 
 * 
 */
void 
Client::start_session(ConsoleLogger logger) {
    get_access_token();
    get_user_principals();
    String host;
    String port = "443";
    Try {
        host = _user_principals.get<String>("streamerInfo.streamerSocketUrl");
    } catch (json::ptree_error const & ptree_bad_path) {
        std::cout << "Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: " << ptree_bad_path.what() << std::endl;
    } finally {
        json::ptree login_request = create_login_request();
        std::stringstream login_text_stream;
        write_json(login_text_stream, login_request);
        String login_text = login_text_stream.str();

        boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};
        websocket_session = std::make_shared<tda::Socket>(ioc_pool.get_executor(), context, logger, login_text);
        websocket_session->open(host.c_str(), port.c_str());
        session_active = true;
    } proceed;

    // json::ptree logout_request = create_logout_request();
    // std::stringstream logout_text_stream;
    // write_json(logout_text_stream, logout_request);
    // String logout_text = logout_text_stream.str();
}

void Client::join_thread_pool() {
    ioc_pool.join();
}

/**
 * @brief Start a WebSocket session quickly with a ticker and fields
 * @author @scawful
 * 
 * @param ticker 
 * @param fields 
 */
void 
Client::start_session(ConsoleLogger logger, String ticker)
{
    String host;
    String port = "443";
    try {
        host = _user_principals.get<String>("streamerInfo.streamerSocketUrl");
    } catch (json::ptree_error const & ptree_bad_path) {
        std::cout << "Start_Session (ptree_bad_path)[streamerInfo.streamerSocketUrl]: " << ptree_bad_path.what() << std::endl;
    }

    pt::ptree login_request = create_login_request();
    pt::ptree service_request = create_service_request(QUOTE, ticker, "0,1,2,3,4,5,6,7,8");

    std::stringstream login_text_stream;
    write_json(login_text_stream, login_request);
    String login_text = login_text_stream.str();

    json::ptree requests;
    json::ptree children;
    std::stringstream requests_text_stream;
    children.push_back(std::make_pair("", service_request));
    children.push_back(std::make_pair("", login_request));
    requests.add_child("requests", children);
    write_json(requests_text_stream, requests);
    String requestsString = requests_text_stream.str();
    std::cout << requestsString << std::endl;

    boost::asio::ssl::context context{boost::asio::ssl::context::tlsv12_client};
    websocket_session = std::make_shared<tda::Socket>(ioc_pool.get_executor(), context, logger, requestsString);
    websocket_session->open(host.c_str(), port.c_str());
    session_active = true;
}

/**
 * @brief Send a request to the current WebSocket session
 * @author @scawful
 * 
 * @param request 
 */
void 
Client::send_session_request(String request) const {
    websocket_session->write(request);
}

void 
Client::send_login_request() {
    get_user_principals();
    pt::ptree login_request = create_login_request();
    std::stringstream login_text_stream;
    pt::write_json(login_text_stream, login_request);
    String login_text = login_text_stream.str();
    websocket_session->write(login_text);
}

void 
Client::send_basic_quote_request(String ticker)
{
    std::stringstream requests_text_stream;
    write_json(requests_text_stream, create_service_request(QUOTE, ticker, "0,1,2,3,4,5,6,7,8"));
    String request_text = requests_text_stream.str();
    websocket_session->write(request_text);
}

/**
 * @brief Send a logout request to the current WebSocket session
 * @author @scawful
 * 
 */
void 
Client::send_logout_request() {
    websocket_session->close();
    pt::ptree logout_request = create_logout_request();
    std::stringstream logout_text_stream;
    pt::write_json(logout_text_stream, logout_request);
    String logout_text = logout_text_stream.str();
    websocket_session->write(logout_text);
}

/**
 * @brief Getter for API access token
 * 
 * @return String 
 */
String 
Client::get_access_token() const {
    return access_token;
}

/**
 * @brief Public retrieval of access token
 * @author @scawful
 * 
 */
void 
Client::fetch_access_token() {
    access_token = parser.parse_access_token(post_access_token());
    has_access_token = true;
}

void 
Client::addAuth(CRString key, CRString token) {
    api_key = key;
    refresh_token = token;
}

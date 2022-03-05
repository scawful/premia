#include "Client.hpp"

using namespace tda;

/**
 * @brief Get User Principals from API endpoint
 *        Parse and store in UserPrincipals object for local use
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

std::string Client::post_access_token() 
{
    CURL *curl;
    CURLcode res;
    std::string response;

    res = curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl)
    {
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
        std::string easy_escape = curl_easy_escape(curl, refresh_token.c_str(), refresh_token.length());
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


std::string Client::get_api_interval_value(int value)
{
    return EnumAPIValues[value];
}

std::string Client::get_api_frequency_type(int value)
{
    return EnumAPIFreq[value];
}

std::string Client::get_api_period_amount(int value)
{
    return EnumAPIPeriod[value];
}

std::string Client::get_api_frequency_amount(int value)
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
bool Client::string_replace(std::string& str, const std::string from, const std::string to)
{
    size_t start = str.find(from);
    if (start == std::string::npos)
        return false;

    str.replace(start, from.length(), to);
    return true;
}

/**
 *
 *
 *
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
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client() 
{
    this->access_token = "";
    this->has_access_token = false;
    this->has_user_principals = false;
    this->refresh_token = REFRESH_TOKEN;
}

/**
 * @brief 
 * 
 * @param endpoint 
 * @return std::string 
 */
std::string Client::send_request(std::string endpoint) 
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

std::string Client::send_authorized_request(std::string endpoint) 
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = NULL;
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

std::string Client::get_price_history(std::string symbol, PeriodType ptype, int period_amt, FrequencyType ftype, int freq_amt, bool ext)
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

std::string Client::get_option_chain(std::string ticker, std::string contractType, std::string strikeCount,
                                     bool includeQuotes, std::string strategy, std::string range,
                                     std::string expMonth, std::string optionType)
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

std::string Client::get_quote(std::string symbol)
{
    std::string url = "https://api.tdameritrade.com/v1/marketdata/{ticker}/quotes?apikey=" + TDA_API_KEY;
    string_replace(url, "{ticker}", symbol);
    return send_request(url);
}

std::string Client::get_account(std::string account_id)
{
    get_user_principals();
    std::string account_url = "https://api.tdameritrade.com/v1/accounts/{accountNum}?fields=positions,orders";
    string_replace(account_url, "{accountNum}", account_id);
    return send_authorized_request(account_url);
}

std::vector<std::string> Client::get_all_account_ids()
{
    std::vector<std::string> accounts;
    fetch_access_token();
    get_user_principals();

    for (auto& array : _user_principals.get_child("accounts")) {
        for (auto& each_element : array.second) {
            if (each_element.first == "accountId") {
                accounts.push_back(each_element.second.get_value<std::string>());
            }
        }
    }

    return accounts;
}

void Client::start_session() 
{
    
}

void Client::send_session_request(std::string request) 
{
    websocket_session->send_message(std::make_shared<std::string const>(request));
}

void Client::send_interrupt_signal() 
{
    websocket_session->interrupt();
}

/**
 * @brief Getter for API access token
 * 
 * @return std::string 
 */
std::string Client::get_access_token()
{
    return access_token;
}

/**
 * @brief 
 * 
 */
void Client::fetch_access_token()
{
    access_token = parser.parse_access_token(post_access_token());
    has_access_token = true;
}

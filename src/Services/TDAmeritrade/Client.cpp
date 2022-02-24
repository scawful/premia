#include "Client.hpp"

/**
 * @brief Get User Principals from API endpoint
 *        Parse and store in UserPrincipals object for local use
 * 
 */
void tda::Client::get_user_principals() 
{
    std::string endpoint = "https://api.tdameritrade.com/v1/userprincipals?fields=streamerSubscriptionKeys,streamerConnectionInfo";
    std::string response = send_authorized_request(endpoint);
    JSONObject::ptree json_principals = parser.read_response(response);
    user_principals = parser.parse_user_principals(json_principals);
}

std::string tda::Client::post_access_token() 
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

size_t tda::Client::json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
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
tda::Client::Client() 
{
    this->access_token = "";
}

/**
 * @brief 
 * 
 * @param endpoint 
 * @return JSONObject 
 */
std::string tda::Client::send_request(std::string endpoint) 
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

std::string tda::Client::send_authorized_request(std::string endpoint) 
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

void tda::Client::start_session() 
{
    
}

void tda::Client::send_session_request(std::string request) 
{
    websocket_session->send_message(std::make_shared<std::string const>(request));
}

void tda::Client::send_interrupt_signal() 
{
    websocket_session->interrupt();
}
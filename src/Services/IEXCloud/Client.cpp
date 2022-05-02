#include "Client.hpp"

using namespace iex;

String Client::current_endpoint()
{
    if (sandbox_mode)
        return this->sandbox_endpoint;
    else
        return this->base_endpoint;
}

size_t Client::json_write_callback(void *contents, size_t size, size_t nmemb, String *s)
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

Client::Client() 
{
    this->base_endpoint = "https://cloud.iexapis.com/";
    this->sandbox_endpoint = "https://sandbox.iexapis.com/stable/";
    this->token_parameter = "?token=" + api_key;
    this->sandbox_mode = true;
}

/**
 * @brief 
 * 
 * @param endpoint 
 * @return String 
 */
String Client::send_request(String endpoint) 
{
    CURL *curl;
    CURLcode res;
    String response;

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
 * @brief 
 * 
 * @param endpoint 
 * @return String 
 */
String Client::send_authorized_request(String endpoint) 
{
    CURL *curl;
    CURLcode res;
    String response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = NULL;
        String auth_bearer = "Authorization: Bearer " + api_key;
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

String Client::get_fund_ownership(String symbol)
{
    String endpoint = current_endpoint() + "stock/" + symbol + "/fund-ownership/" + token_parameter;
    return send_request(endpoint);
}

String Client::get_insider_transactions(String symbol)
{
    String endpoint = current_endpoint() + "stock/" + symbol + "/insider-transactions/" + token_parameter;
    return send_request(endpoint);
}
#include "Client.hpp"

using namespace iex;

std::string Client::current_endpoint()
{
    if (sandbox_mode)
        return this->sandbox_endpoint;
    else
        return this->base_endpoint;
}

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

Client::Client() 
{
    this->base_endpoint = "https://cloud.iexapis.com/";
    this->sandbox_endpoint = "https://sandbox.iexapis.com/stable/";
    this->token_parameter = "?token=" + IEX_KEY;
    this->sandbox_mode = true;
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

/**
 * @brief 
 * 
 * @param endpoint 
 * @return std::string 
 */
std::string Client::send_authorized_request(std::string endpoint) 
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = NULL;
        std::string auth_bearer = "Authorization: Bearer " + IEX_KEY;
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

std::string Client::get_fund_ownership(std::string symbol)
{
    std::string endpoint = current_endpoint() + "stock/" + symbol + "/fund-ownership/" + token_parameter;
    return send_request(endpoint);
}

std::string Client::get_insider_transactions(std::string symbol)
{
    std::string endpoint = current_endpoint() + "stock/" + symbol + "/insider-transactions/" + token_parameter;
    return send_request(endpoint);
}
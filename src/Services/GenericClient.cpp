#include "GenericClient.hpp"

size_t GenericClient::json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
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
 * @brief 
 * 
 * @param endpoint 
 * @return std::string 
 */
std::string GenericClient::send_request(std::string endpoint) 
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


GenericClient::GenericClient() 
{
    
}

std::string GenericClient::get_spx_gex()
{
    std::string response = send_request("https://squeezemetrics.com/monitor/download/SPX.csv");
    return response;
}

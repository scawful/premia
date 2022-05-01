#include "GenericClient.hpp"

size_t GenericClient::json_write_callback(const char * contents, size_t size, size_t nmemb, std::string *s)
{
    size_t new_length = size * nmemb;
    try {
        s->append(contents, new_length);
    } catch(const std::bad_alloc & e) {
        std::cout << e.what() << std::endl;
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
std::string GenericClient::send_request(const std::string & endpoint) const
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, json_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);

    return response;
}


GenericClient::GenericClient()=default;

std::string GenericClient::get_spx_gex() const
{
    std::string response = send_request("https://squeezemetrics.com/monitor/download/SPX.csv");
    return response;
}

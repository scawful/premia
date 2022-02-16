#include "Client.hpp"

void Client::get_user_principals() 
{
    
}

void Client::post_access_token() 
{
    
}

void Client::post_account_authorization() 
{
    
}

size_t json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

/**
 * @brief Construct a new Client:: Client object
 * 
 */
Client::Client() 
{
    
}

/**
 * @brief 
 * 
 * @param endpoint 
 * @return JSONObject 
 */
JSONObject Client::send_request(std::string endpoint) 
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(filename.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    // ==========

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

JSONObject Client::send_authorized_request(std::string endpoint) 
{
    
}

void Client::start_session() 
{
    
}

void Client::send_session_request() 
{
    
}

void Client::send_interrupt_signal() 
{
    
}

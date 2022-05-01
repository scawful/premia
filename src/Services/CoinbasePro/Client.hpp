#ifndef CBP_Client_hpp
#define CBP_Client_hpp

#include <iostream>
#include "Boost.hpp"
#include "Curl.hpp"

namespace cbp
{
    class Client 
    {
    private:
        std::string api_key;
        std::string secret_key;
        std::string passphrase;
        std::string endpoint_url;

        unsigned int hash_length;
        std::array<unsigned char, EVP_MAX_MD_SIZE> hash;

        static size_t json_write_callback(const char * contents, size_t size, size_t nmemb, std::string * s);
        std::string get_server_time() const;

    public:
        Client( std::string_view api, std::string_view secret, std::string_view passphrase, bool sandbox );
        ~Client();

        std::string send_request(std::string_view request);
        void send_request(std::string request, std::string method, std::string body);

    };
}

#endif
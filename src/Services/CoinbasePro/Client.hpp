#ifndef CBP_Client_hpp
#define CBP_Client_hpp

#include <iostream>
#include "Metatypes.hpp"
#include "Library/Boost.hpp"
#include "Library/Curl.hpp"

namespace cbp
{
    class Client 
    {
    private:
        String api_key;
        String secret_key;
        String passphrase;
        String endpoint_url;

        unsigned int hash_length;
        std::array<unsigned char, EVP_MAX_MD_SIZE> hash;

        static size_t json_write_callback(const char * contents, size_t size, size_t nmemb, String * s);
        String get_server_time() const;

    public:
        Client(CRString api, CRString secret, CRString passphrase, bool sandbox);
        ~Client();

        String send_request(CRString request);
        void send_request(String request, String method, String body);

    };
}

#endif
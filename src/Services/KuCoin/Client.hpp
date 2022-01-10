#ifndef KuCoin_Client_hpp
#define KuCoin_Client_hpp

#include "../../core.hpp"

namespace kc
{
    class Client 
    {
    private:
        std::string _api_key;
        std::string _secret_key;
        std::string _key_version;
        std::string _passphrase;
        std::string _endpoint_url;

        unsigned int _hash_length;
        std::array<unsigned char, EVP_MAX_MD_SIZE> _hash;

        long get_server_time();

    public:
        Client();
        Client( std::string api_key, std::string secret_key, std::string key_version, std::string passphrase );
        ~Client();

        void send_request( std::string filename, std::string request );
        void send_request( std::string filename, std::string request, std::string method, std::string body );

    };
}

#endif
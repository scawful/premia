#ifndef CBP_Client_hpp
#define CBP_Client_hpp

#include "../../core.hpp"

namespace cbp
{
    class Client 
    {
    private:
        std::string _api_key;
        std::string _secret_key;
        std::string _passphrase;
        std::string _endpoint_url;

        unsigned int _hash_length;
        std::array<unsigned char, EVP_MAX_MD_SIZE> _hash;

        std::string get_server_time();

    public:
        Client( std::string api_key, std::string secret_key, std::string passphrase, bool sandbox );
        ~Client();

        void send_request( std::string filename, std::string request );
        void send_request( std::string filename, std::string request, std::string method, std::string body );

    };
}

#endif
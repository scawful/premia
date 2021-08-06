#ifndef CoinbasePro_hpp
#define CoinbasePro_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "CoinbasePro/Account.hpp"
#include "CoinbasePro/Client.hpp"

namespace cbp
{
    class CoinbasePro
    {
    private:
        boost::shared_ptr<Client> http_client;
        std::time_t _timestamp;
        std::string _message;
        std::string _body;
        std::string _method;
        std::string _request_path;
        std::string _endpoint_url;
        std::string _access_signature;

        unsigned int _hash_length;
        std::array<unsigned char, EVP_MAX_MD_SIZE> _hash;

        void get_account_data( std::string filename );

    public:
        CoinbasePro();
        ~CoinbasePro();

        boost::shared_ptr<cbp::Account> list_accounts();

    };
}

#endif
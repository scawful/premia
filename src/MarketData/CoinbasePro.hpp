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

    public:
        CoinbasePro();
        ~CoinbasePro();

        boost::shared_ptr<cbp::Account> list_accounts();

    };
}

#endif
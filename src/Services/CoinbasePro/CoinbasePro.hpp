#ifndef CoinbasePro_hpp
#define CoinbasePro_hpp

#include "core.hpp"
#include "apikey.hpp"
#include "Account.hpp"
#include "Client.hpp"
#include "Product.hpp"

namespace cbp
{
    class CoinbasePro
    {
    private:
        float total_deposits_usd;
        boost::shared_ptr<Client> http_client;

    public:
        CoinbasePro();
        ~CoinbasePro();

        boost::shared_ptr<cbp::Account> list_accounts();
        boost::shared_ptr<cbp::Product> get_product_ticker( const std::string & symbol );
        float get_deposits();

    };
}

#endif
#ifndef CBP_hpp
#define CBP_hpp

#include "../src/Services/CoinbasePro/Client.hpp"
#include "../src/Services/CoinbasePro/Parser.hpp"
#include "../src/Services/CoinbasePro/Product.hpp"
#include "../src/Services/CoinbasePro/Account.hpp"

namespace cbp
{
    class CBP 
    {
    private:
        CBP()=default;
        Parser parser;
        Account account;
        Client client;

    public:
        CBP(CBP const&)             = delete;
        void operator=(CBP const&)  = delete;
        static CBP& getInstance() {
            static CBP instance;    
            return instance;
        }

        auto getAccounts()
            -> Account {
            String request = "/accounts";
            String response = client.send_request(request);
            auto json = parser.read_response(response);
            account = parser.parse_account_data(json);
            return account;
        }

        auto getProductTicker(CRString symbol)
            -> Product {
            Product product;
            String request = "/products/" + symbol + "-USD/ticker";
            String response = client.send_request(request);
            auto json = parser.read_response(response);
            product = parser.parse_product_data(json);
            return product;
        }

        auto getTransfers()
            -> Transfers {
            String response = client.send_request("/transfers");
            auto json = parser.read_response(response);
            Transfers transfers = parser.parse_transfer_data(json);
            return transfers;
        }
    };
}


#endif
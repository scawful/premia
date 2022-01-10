#ifndef KuCoin_hpp
#define KuCoin_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "KuCoin/Parser.hpp"
#include "KuCoin/Client.hpp"
#include "KuCoin/Account.hpp"
#include "KuCoin/MarginAccount.hpp"

namespace kc
{
    class KuCoin 
    {
    private:
        Client client;
        Parser parser;

    public:
        KuCoin();

        Account list_accounts();
        MarginAccount get_margin_account();
    };
}

#endif 
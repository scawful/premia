#ifndef KuCoin_hpp
#define KuCoin_hpp

#include "../core.hpp"
#include "../apikey.hpp"
#include "KuCoin/Parser.hpp"
#include "KuCoin/Client.hpp"
#include "KuCoin/Account.hpp"

namespace kc
{
    class KuCoin 
    {
    private:
        Client client;

    public:
        KuCoin();

        Account list_accounts();
    };
}

#endif 
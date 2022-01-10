#ifndef KuCoin_MarginAccount_hpp
#define KuCoin_MarginAccount_hpp

#include "../../core.hpp"

namespace kc
{
    class MarginAccount
    {
    private:
        std::string currency;
        double available_balance;
        double hold_balance;
        double liability;
        double max_borrow_size;
        double total_balance;

    public:
        MarginAccount();

    };
}

#endif

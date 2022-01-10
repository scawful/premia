#ifndef KuCoin_Account_hpp
#define KuCoin_Account_hpp

#include "../../core.hpp"

namespace kc 
{
    class Account
    {
    private:

    public:
        Account();
        Account(boost::property_tree::ptree data);
    };
}

#endif
#ifndef CBP_Account_hpp
#define CBP_Account_hpp

#include "../../core.hpp"

namespace cbp
{
    class Account 
    {
    private:
        boost::property_tree::ptree accountData;

        void initVariables();

    public:
        Account( boost::property_tree::ptree account_data );

    };
}

#endif
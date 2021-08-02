#include "Account.hpp"

namespace cbp
{
    void Account::initVariables()
    {

    }

    Account::Account( boost::property_tree::ptree account_data )
    {
        accountData = account_data;
        initVariables();
    }
}
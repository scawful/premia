/* =============== Account Class =============== */
#include "Account.hpp"

void tda::Account::initVariables()
{
    for ( auto& account_it: accountData )
    {
        if ( account_it.first == "positions" )
        {
            for ( auto& positions_it: account_it.second )
            {
                
            }
        }
        else if ( account_it.first == "currentBalances" )
        {
            currentBalanceMap[ account_it.first ] = account_it.second.get_value<std::string>();
        }
        else
        {
            accountInfoMap[ account_it.first ] = account_it.second.get_value<std::string>();
        }

        std::cout << account_it.first << " ::: " << account_it.second.get_value<std::string>() << std::endl;
    }
}

tda::Account::Account( boost::property_tree::ptree account_data )
{
    accountData = account_data;
    initVariables();
}

std::string tda::Account::getAccountVariable( std::string variable )
{
    return accountInfoMap[ variable ];
}
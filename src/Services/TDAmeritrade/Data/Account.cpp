/* =============== Account Class =============== */
#include "Account.hpp"

using namespace tda;

size_t tda::Account::get_position_vector_size()
{
    return positions_vector.size();
}

void Account::add_position(std::unordered_map<std::string, std::string> position)
{
    positions_vector.push_back(position);
}

void Account::add_balance(PositionBalances balance)
{
    position_balances.push_back(balance);
}

void Account::set_account_variable(std::string key, std::string value)
{
    account_info[key] = value;
}

void Account::set_balance_variable(std::string key, std::string value)
{
    current_balances[key] = value;
}

std::string tda::Account::get_account_variable( std::string variable )
{
    return account_info[ variable ];
}

std::string tda::Account::get_balance_variable( std::string variable )
{
    return current_balances[variable];
}

std::unordered_map<std::string, std::string> tda::Account::get_position( int index )
{
    return positions_vector[index];
}

std::string tda::Account::get_position_balances( std::string symbol, std::string variable )
{
    std::string found_item;
    for ( int i = 0; i < position_balances.size(); i++ )
    {
        if ( position_balances[i].symbol == symbol )
        {
            found_item = position_balances[i].balances[ variable ];
        }
    }

    return found_item;
}
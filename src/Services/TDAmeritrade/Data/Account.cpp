/* =============== Account Class =============== */
#include "Account.hpp"

using namespace tda;

size_t tda::Account::get_position_vector_size()
{
    return positions_vector.size();
}

void Account::add_position(StringMap position)
{
    positions_vector.push_back(position);
}

void Account::add_balance(PositionBalances balance)
{
    position_balances.push_back(balance);
}

void Account::set_account_variable(String key, String value)
{
    account_info[key] = value;
}

void Account::set_balance_variable(String key, String value)
{
    current_balances[key] = value;
}

String tda::Account::get_account_variable( String variable )
{
    return account_info[ variable ];
}

String tda::Account::get_balance_variable( String variable )
{
    return current_balances[variable];
}

StringMap tda::Account::get_position( int index )
{
    return positions_vector[index];
}

String tda::Account::get_position_balances( String symbol, String variable )
{
    String found_item;
    for ( int i = 0; i < position_balances.size(); i++ )
    {
        if ( position_balances[i].symbol == symbol )
        {
            found_item = position_balances[i].balances[ variable ];
        }
    }

    return found_item;
}
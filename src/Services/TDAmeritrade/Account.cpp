/* =============== Account Class =============== */
#include "Account.hpp"

void tda::Account::initVariables()
{
    for ( auto& class_it: accountData )
    {
        for ( auto& account_it: class_it.second )
        {
            if ( account_it.first == "positions" )
            {
                for ( auto& position_list_it: account_it.second )
                {
                    PositionBalances new_position_balance;

                    for ( auto& positions_it: position_list_it.second )
                    {
                        //std::cout << positions_it.first << " ::: " << positions_it.second.get_value<std::string>() << std::endl;

                        new_position_balance.balances[ positions_it.first ] = positions_it.second.get_value<std::string>();
            
                        std::unordered_map<std::string, std::string> pos_field;
                        std::unordered_map<std::string, std::string> instrument;

                        for ( auto& field_it: positions_it.second )
                        {
                            //std::cout << field_it.first << " :::: " << field_it.second.get_value<std::string>() << std::endl;
                            if ( field_it.first == "symbol" )
                                new_position_balance.symbol = field_it.second.get_value<std::string>();

                            pos_field[ field_it.first ] = field_it.second.get_value<std::string>();
                        }

                        positions_vector.push_back( pos_field );
                    }
                    position_balances.push_back( new_position_balance );
                }
            }
            else if ( account_it.first == "currentBalances" )
            {
                for ( auto& balance_it: account_it.second )
                {
                    current_balances[ balance_it.first ] = balance_it.second.get_value<std::string>();
                } 
            }
            else
            {
                account_info[ account_it.first ] = account_it.second.get_value<std::string>();
            }
        }    
    }

}

tda::Account::Account()
{
    
}

tda::Account::Account( boost::property_tree::ptree account_data )
{
    accountData = account_data;
    initVariables();
}

size_t tda::Account::get_position_vector_size()
{
    return positions_vector.size();
}

std::string tda::Account::get_account_variable( std::string variable )
{
    return account_info[ variable ];
}

std::string tda::Account::get_balance_variable( std::string variable )
{
    return current_balances[ variable ];
}

std::unordered_map<std::string, std::string> tda::Account::get_position( int index )
{
    return positions_vector[ index ];
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
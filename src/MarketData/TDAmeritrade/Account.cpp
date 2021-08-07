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
                for ( auto& position_list_it: account_it.second)
                {
                    for ( auto& positions_it: position_list_it.second )
                    {
                        std::cout << positions_it.first << " ::: " << positions_it.second.get_value<std::string>() << std::endl;

                        std::unordered_map<std::string, std::string> pos_field;
                        std::unordered_map<std::string, std::string> instrument;

                        for ( auto& field_it: positions_it.second )
                        {
                            if ( field_it.first == "instrument" )
                            {
                                for ( auto& instrument_it: field_it.second )
                                {
                                    instrument[ instrument_it.first ] = instrument_it.second.get_value<std::string>();
                                }
                                instrument_vector.push_back( instrument );
                            }

                            pos_field[ field_it.first ] = field_it.second.get_value<std::string>();
                        }
                        
                        positions_vector.push_back( pos_field );
                    }
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

            //std::cout << account_it.first << " ::: " << account_it.second.get_value<std::string>() << std::endl;
        }    
    }

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

std::unordered_map<std::string, std::string> tda::Account::get_position_instrument( int index )
{
    return instrument_vector[ index ];
}
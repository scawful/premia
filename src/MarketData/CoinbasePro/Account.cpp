#include "Account.hpp"

namespace cbp
{
    void Account::initVariables()
    {
        for ( auto& account_it : accountData )
        {
            std::unordered_map< std::string, std::string > new_position_data;
            for ( auto& data_it: account_it.second )
            {
                new_position_data[data_it.first] = data_it.second.get_value<std::string>();
                
                SDL_Log("%s", data_it.second.get_value<std::string>().c_str() );
            }
            accounts_vector.push_back( new_position_data );
        }
    }

    Account::Account( boost::property_tree::ptree account_data )
    {
        accountData = account_data;
        initVariables();
    }

    std::unordered_map<std::string, std::string> Account::get_position( std::string ticker )
    {
        for ( auto fields: accounts_vector )
        {
            for ( auto& accounts_vector_it: fields )
            {
                if ( accounts_vector_it.second == ticker )
                {
                    return fields;
                }
            }
        }

        return accounts_vector[0];
    }

}
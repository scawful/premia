#include "Account.hpp"

namespace kc
{
    Account::Account() 
    {
        
    }

    Account::Account(boost::property_tree::ptree data ) 
    {
        for ( auto & each : data )
        {
            std::cout << each.first << " " << each.second.get_value<std::string>();
        }
    }
}

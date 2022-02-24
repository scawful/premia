#include "UserPrincipals.hpp"

tda::UserPrincipals::UserPrincipals() 
{
    
}

void tda::UserPrincipals::set_account_data(std::unordered_map<std::string, std::string> data)
{
    this->account_data = data;
}
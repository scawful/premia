#include "UserPrincipals.hpp"

using namespace tda;

tda::UserPrincipals::UserPrincipals() 
{
    
}

void tda::UserPrincipals::set_account_data(std::unordered_map<std::string, std::string> data)
{
    this->account_data = data;
}

std::string UserPrincipals::get_account_data(std::string key)
{
    return account_data[key];
}
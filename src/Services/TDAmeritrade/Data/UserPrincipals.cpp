#include "UserPrincipals.hpp"

using namespace tda;

UserPrincipals::UserPrincipals() 
{
    
}

void UserPrincipals::add_account_data(std::unordered_map<std::string, std::string> data)
{
    accounts_array.push_back(data);
}

void UserPrincipals::set_account_data(std::unordered_map<std::string, std::string> data)
{
    this->account_data = data;
}

std::unordered_map<std::string, std::string> UserPrincipals::get_account_data_array(int i)
{
    return accounts_array[i];
}

std::string UserPrincipals::get_account_data(std::string key)
{
    return account_data[key];
}
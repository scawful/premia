#include "UserPrincipals.hpp"
namespace premia {
using namespace tda;

UserPrincipals::UserPrincipals() = default;

void UserPrincipals::add_account_data(const std::unordered_map<std::string, std::string>& data) {
  accounts_array.push_back(data);
}

void UserPrincipals::set_account_data(const std::unordered_map<std::string, std::string>& data) {
  this->account_data = data;
}

std::unordered_map<std::string, std::string> UserPrincipals::get_account_data_array(int i) {
  return accounts_array[i];
}

std::string UserPrincipals::get_account_data(const std::string &key) {
  return account_data[key];
}
}  // namespace premia
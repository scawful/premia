#include "UserPrincipals.hpp"
namespace Premia {
using namespace tda;

UserPrincipals::UserPrincipals() = default;

void UserPrincipals::add_account_data(const StringMap& data) {
  accounts_array.push_back(data);
}

void UserPrincipals::set_account_data(const StringMap& data) {
  this->account_data = data;
}

StringMap UserPrincipals::get_account_data_array(int i) {
  return accounts_array[i];
}

String UserPrincipals::get_account_data(CRString key) {
  return account_data[key];
}
}  // namespace Premia
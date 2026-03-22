#ifndef Plaid_Account_hpp
#define Plaid_Account_hpp

#include <string>

#include "Balance.hpp"

namespace premia {
namespace plaid {

struct Account {
  std::string account_id;
  std::string name;
  std::string official_name;
  std::string type;     // depository, credit, loan, investment, other
  std::string subtype;  // checking, savings, credit card, etc.
  std::string mask;     // last 4 digits
  Balance balances;
};

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Account_hpp

#ifndef Plaid_Transaction_hpp
#define Plaid_Transaction_hpp

#include <string>
#include <vector>

namespace premia {
namespace plaid {

struct Transaction {
  std::string transaction_id;
  std::string account_id;
  double amount = 0.0;   // positive = debit, negative = credit
  std::string date;       // ISO 8601: YYYY-MM-DD
  std::string name;       // merchant or description
  std::string merchant_name;
  std::vector<std::string> category;
  std::string category_id;
  std::string iso_currency_code;
  bool pending = false;
};

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Transaction_hpp

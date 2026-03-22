#ifndef Plaid_Balance_hpp
#define Plaid_Balance_hpp

#include <string>

namespace premia {
namespace plaid {

struct Balance {
  double current   = 0.0;
  double available = 0.0;
  double limit     = 0.0;
  std::string iso_currency_code;
};

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Balance_hpp

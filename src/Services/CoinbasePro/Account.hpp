#ifndef CBP_Account_hpp
#define CBP_Account_hpp

#include "Premia.hpp"
namespace Premia {
namespace cbp {
class Account {
 private:
  ArrayList<StringMap> accounts_vector;

 public:
  Account() = default;
  StringMap get_position(CRString ticker);
};
}  // namespace cbp
}  // namespace Premia
#endif
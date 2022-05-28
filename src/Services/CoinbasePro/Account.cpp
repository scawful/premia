#include "Account.hpp"
namespace Premia {
namespace cbp {
/**
 * @brief Get an account positions
 *
 * @param ticker
 * @return StringMap
 */
StringMap Account::get_position(CRString ticker) {
  for (const auto& fields : accounts_vector) {
    for (const auto& [key, value] : fields) {
      if (value == ticker) {
        return fields;
      }
    }
  }
  return accounts_vector[0];
}

}  // namespace cbp
}  // namespace Premia
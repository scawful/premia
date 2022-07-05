#ifndef Utils_hpp
#define Utils_hpp

#include "Metatypes.hpp"

namespace premia {
namespace Utils {

/**
 * @brief Replace a substring within a string with given parameter
 * @author @scawful
 *
 * @param str
 * @param from
 * @param to
 * @return true
 * @return false
 */
static auto string_replace(RString str, CRString from, CRString to) -> bool {
  size_t start = str.find(from);
  if (start == String::npos) return false;

  str.replace(start, from.length(), to);
  return true;
}
}  // namespace Utils
}  // namespace premia

#endif
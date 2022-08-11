#include "Product.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace premia {
namespace cbp {
float Product::get_current_price() const { return current_price; }
}  // namespace cbp
}  // namespace premia
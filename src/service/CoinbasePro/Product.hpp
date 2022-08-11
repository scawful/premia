#ifndef Product_hpp
#define Product_hpp

#include <vector>
#include <unordered_map>
#include <string>

namespace premia {
namespace cbp {
class Product {
 private:
  std::vector<std::unordered_map<std::string, std::string>> accounts_vector;

  float current_price;

  void initVariables();

 public:
  Product() = default;

  float get_current_price() const;
};
}  // namespace cbp
}  // namespace premia

#endif
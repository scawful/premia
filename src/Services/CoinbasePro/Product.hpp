#ifndef Product_hpp
#define Product_hpp

#include "Premia.hpp"

namespace premia {
namespace cbp {
class Product {
 private:
  ArrayList<StringMap> accounts_vector;

  float current_price;

  void initVariables();

 public:
  Product() = default;

  float get_current_price() const;
};
}  // namespace cbp
}  // namespace premia

#endif
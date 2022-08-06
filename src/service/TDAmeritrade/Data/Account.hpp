#ifndef Account_hpp
#define Account_hpp

#include <boost/lexical_cast.hpp>

#include "PricingStructures.hpp"
#include "metatypes.h"

namespace premia {
namespace tda {
struct PositionBalances {
  std::string symbol;
  std::unordered_map<std::string, std::string> balances;
  int longQuantity;
  int averagePrice;
  double dayProfitLoss;
};

class Account {
 private:
  int num_positions;
  std::string account_id;
  std::unordered_map<std::string, std::string> account_info;
  std::unordered_map<std::string, std::string> current_balances;
  std::vector<std::unordered_map<std::string, std::string>> positions_vector;

  std::vector<PositionBalances> position_balances;

 public:
  Account() = default;

  void add_position(std::unordered_map<std::string, std::string> position);
  void add_balance(PositionBalances balance);
  void set_account_variable(std::string key, std::string value);
  void set_balance_variable(std::string key, std::string value);
  void set_primary_account_id(const std::string &key);

  size_t get_position_vector_size();
  std::string get_account_variable(std::string variable);
  std::string get_balance_variable(std::string variable);
  std::unordered_map<std::string, std::string> get_position(int index);
  std::string get_position_balances(std::string symbol, std::string variable);
};

}  // namespace tda
}  // namespace premia
#endif
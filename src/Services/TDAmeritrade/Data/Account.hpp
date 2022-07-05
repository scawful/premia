#ifndef Account_hpp
#define Account_hpp

#include <boost/lexical_cast.hpp>

#include "Metatypes.hpp"
#include "PricingStructures.hpp"


namespace premia {
namespace tda {
struct PositionBalances {
  String symbol;
  StringMap balances;
  int longQuantity;
  int averagePrice;
  double dayProfitLoss;
};

class Account {
 private:
  int num_positions;
  String account_id;
  StringMap account_info;
  StringMap current_balances;
  ArrayList<StringMap> positions_vector;

  ArrayList<PositionBalances> position_balances;

 public:
  Account() = default;

  void add_position(StringMap position);
  void add_balance(PositionBalances balance);
  void set_account_variable(String key, String value);
  void set_balance_variable(String key, String value);
  void set_primary_account_id(CRString key);

  size_t get_position_vector_size();
  String get_account_variable(String variable);
  String get_balance_variable(String variable);
  StringMap get_position(int index);
  String get_position_balances(String symbol, String variable);
};

}  // namespace tda
}  // namespace premia
#endif
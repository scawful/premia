/* =============== Account Class =============== */
#include "Account.hpp"
namespace premia {
namespace tda {

size_t tda::Account::get_position_vector_size() {
  return positions_vector.size();
}

void Account::add_position(std::unordered_map<std::string, std::string> position) {
  positions_vector.push_back(position);
}

void Account::add_balance(PositionBalances balance) {
  position_balances.push_back(balance);
}

void Account::set_account_variable(std::string key, std::string value) {
  account_info[key] = value;
}

void Account::set_balance_variable(std::string key, std::string value) {
  if (!current_balances.count(key)) {
    current_balances[key] = value;
  } else {
    try {
      double num;
      num = boost::lexical_cast<double>(value);
      double balance = boost::lexical_cast<double>(current_balances[key]);
      num += balance;
      std::string newEntry = std::to_string(num);
      current_balances[key] = newEntry.substr(0, newEntry.find('.') + 3);
    } catch (const boost::wrapexcept<boost::bad_lexical_cast>& e) {
      // it's fine
      current_balances[key] = value;
    }
  }
}

void Account::set_primary_account_id(const std::string &key) { account_id = key; }

std::string tda::Account::get_account_variable(std::string variable) {
  return account_info[variable];
}

std::string tda::Account::get_balance_variable(std::string variable) {
  return current_balances[variable];
}

std::unordered_map<std::string, std::string> tda::Account::get_position(int index) {
  return positions_vector[index];
}

std::string tda::Account::get_position_balances(std::string symbol, std::string variable) {
  std::string found_item;
  for (int i = 0; i < position_balances.size(); i++) {
    if (position_balances[i].symbol == symbol) {
      found_item = position_balances[i].balances[variable];
    }
  }

  return found_item;
}
}  // namespace tda
}  // namespace premia
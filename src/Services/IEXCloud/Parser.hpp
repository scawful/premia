#ifndef IEX_Parser_hpp
#define IEX_Parser_hpp

#include "Data/FundOwnership.hpp"
#include "Data/InsiderTransactions.hpp"
#include "Library/Boost.hpp"

namespace Premia {
namespace iex {
class Parser {
 private:
  void log_response(String title, json::ptree data);
  json::ptree read_response(String response);

 public:
  Parser();

  ArrayList<FundOwnership> parse_fund_ownership(String response);
  ArrayList<InsiderTransactions> parse_insider_transactions(String response);
};
}  // namespace iex
}  // namespace Premia

#endif
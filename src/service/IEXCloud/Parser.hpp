#ifndef IEX_Parser_hpp
#define IEX_Parser_hpp

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "Data/FundOwnership.hpp"
#include "Data/InsiderTransactions.hpp"
#include "metatypes.h"

namespace premia {
namespace iex {

namespace json = boost::property_tree;

class Parser {
 private:
  void log_response(std::string title, json::ptree data);
  json::ptree read_response(std::string response);

 public:
  Parser();

  std::vector<FundOwnership> parse_fund_ownership(std::string response);
  std::vector<InsiderTransactions> parse_insider_transactions(std::string response);
};
}  // namespace iex
}  // namespace premia

#endif
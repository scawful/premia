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
  void log_response(String title, json::ptree data);
  json::ptree read_response(String response);

 public:
  Parser();

  ArrayList<FundOwnership> parse_fund_ownership(String response);
  ArrayList<InsiderTransactions> parse_insider_transactions(String response);
};
}  // namespace iex
}  // namespace premia

#endif
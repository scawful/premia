#include "Parser.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

#include "Data/FundOwnership.hpp"
#include "Data/InsiderTransactions.hpp"
#include "Metatypes.hpp"

namespace premia {
namespace iex {

void Parser::log_response(String title, json::ptree data) {
  std::time_t now = std::time(0);
  String filename = title + boost::lexical_cast<String>(now) + ".json";
  std::ofstream file(filename);

  // boost::filesystem::path dir("data");

  // if(!(boost::filesystem::exists(dir))){
  //     std::cout<<"Doesn't Exists"<<std::endl;

  //     if (boost::filesystem::create_directory(dir))
  //         std::cout << "....Successfully Created !" << std::endl;
  // }

  try {
    write_json(file, data);
  } catch (std::exception &json_parser_error) {
    std::cout << "iex::Parser - " << json_parser_error.what() << std::endl;
  }
}

json::ptree Parser::read_response(String response) {
  std::cout << response << std::endl;
  std::istringstream json_response(response);
  json::ptree property_tree;

  try {
    read_json(json_response, property_tree);
  } catch (std::exception &json_parser_error) {
    std::cout << json_parser_error.what() << std::endl;
  }

  return property_tree;
}

Parser::Parser() {}

ArrayList<FundOwnership> Parser::parse_fund_ownership(String response) {
  ArrayList<FundOwnership> funds_array;
  json::ptree fund_ownership_json = read_response(response);
  log_response("fund-ownership", fund_ownership_json);

  for (auto &outer_list : fund_ownership_json) {
    int i = 0;
    FundOwnership fund_ownership;
    for (auto &each : outer_list.second) {
      std::cout << each.first << " : " << each.second.get_value<String>()
                << std::endl;
      if (each.first == "adjHolding") {
        fund_ownership.setAdjustedMarketValue(each.second.get_value<String>());
      } else if (each.first == "adjMv") {
        fund_ownership.setAdjustedHoldings(each.second.get_value<String>());
      } else if (each.first == "entityProperName") {
        fund_ownership.setEntityProperName(each.second.get_value<String>());
      } else if (each.first == "reportedHolding") {
        fund_ownership.setReportedHolding(each.second.get_value<String>());
      } else if (each.first == "reportedMv") {
        fund_ownership.setReportedMarketValue(each.second.get_value<String>());
      } else if (each.first == "symbol") {
        fund_ownership.setSymbol(each.second.get_value<String>());
      } else if (each.first == "date") {
        fund_ownership.setReportDate(each.second.get_value<time_t>());
      } else if (each.first == "id") {
        //
      } else if (each.first == "updated") {
        //
      }

      i++;

      if (i == 10) {
        funds_array.push_back(fund_ownership);
        i = 0;
      }
    }
  }
  return funds_array;
}

ArrayList<InsiderTransactions> Parser::parse_insider_transactions(
    String response) {
  ArrayList<InsiderTransactions> transactions_array;
  return transactions_array;
}
}  // namespace iex
}  // namespace premia
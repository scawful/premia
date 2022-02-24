#include "Parser.hpp"

using namespace iex;

JSONObject::ptree Parser::read_response(std::string response) 
{
    SDL_Log("%s", response.c_str());
    std::istringstream json_response(response);
    JSONObject::ptree property_tree;

    try {
        read_json(json_response, property_tree);
    }
    catch (std::exception &json_parser_error) {
        SDL_Log("%s", json_parser_error.what());
    }

    return property_tree; 
}


Parser::Parser() 
{
    
}

std::vector<FundOwnership> Parser::parse_fund_ownership(std::string response)
{
    std::vector<FundOwnership> funds_array;
    
    JSONObject::ptree fund_ownership_json = read_response(response);
    for ( auto & outer_list : fund_ownership_json )
    {
        int i = 0;
        FundOwnership fund_ownership;
        for ( auto & each : outer_list.second )
        {
            
            std::cout << each.first << " : " << each.second.get_value<std::string>() << std::endl;
            if ( each.first == "adjHolding") {
                fund_ownership.setAdjustedMarketValue(each.second.get_value<std::string>());
            } else if ( each.first == "adjMv" ) {
                fund_ownership.setAdjustedHoldings(each.second.get_value<std::string>());
            } else if ( each.first == "entityProperName" ) {
                fund_ownership.setEntityProperName(each.second.get_value<std::string>());
            } else if ( each.first == "reportedHolding" ) {
                fund_ownership.setReportedHolding(each.second.get_value<std::string>());
            } else if ( each.first == "reportedMv" ) {
                fund_ownership.setReportedMarketValue(each.second.get_value<std::string>());
            } else if ( each.first == "symbol" ) {
                fund_ownership.setSymbol(each.second.get_value<std::string>());
            } else if ( each.first == "date") {
                fund_ownership.setReportDate(each.second.get_value<time_t>());
            } else if ( each.first == "id") {
                // 
            } else if ( each.first == "updated") {
                // 
            }

            i++;

            if ( i == 10 ) {
                funds_array.push_back(fund_ownership);
                i = 0;
            }    
        }
        
    }
    return funds_array;
}

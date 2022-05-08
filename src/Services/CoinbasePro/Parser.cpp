#include "Parser.hpp"

using namespace cbp;

/**
 * @brief Take a response from the API as argument and read it into a boost::property_tree
 * @author @scawful
 * 
 * @param response 
 * @return json::ptree 
 */
json::ptree 
Parser::read_response(CRString response) const
{
    std::istringstream json_response(response);
    json::ptree property_tree;

    try {
        read_json(json_response, property_tree);
    } catch (const json::ptree_error & json_parser_error) {
        std::cout << "Parser::read_response: " << json_parser_error.what() << std::endl;
    }

    return property_tree;
}

Account 
Parser::parse_account_data(const json::ptree & data) const
{
    Account account;
    for (const auto & [dataKey, dataValue] : data) {
        // this is where you would parse the JSON for an account 
    }
    
    return account;
}

Product 
Parser::parse_product_data(const json::ptree & data) const
{
    Product product;
    for (const auto & [dataKey, dataValue] : data) {
        // this is where you would parse the JSON for a product  
    }
    
    return product;
}

Transfers 
Parser::parse_transfer_data(const json::ptree & data) const
{
    Transfers transfers;
    float total_deposits_usd = 0.f;
    for (const auto & [key,value] : data) {
        for ( const auto & [depositKey, depositValue] : value ) {
            if ( depositKey == "amount" ) {
                total_deposits_usd += boost::lexical_cast<float>(depositValue.get_value<float>());
            }
        }
    }
    transfers.depositsUSD = total_deposits_usd;
    return transfers;  
}

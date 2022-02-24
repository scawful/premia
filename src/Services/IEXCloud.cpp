#include "IEXCloud.hpp"

using namespace iex;

IEXCloud::IEXCloud() 
{
    
}

/**
 * @brief 
 * 
 * @param symbol 
 * @return FundOwnership 
 */
std::vector<FundOwnership> IEXCloud::getFundOwnership(std::string symbol)
{
    std::string response = client.get_fund_ownership(symbol);
    return parser.parse_fund_ownership(response);
}

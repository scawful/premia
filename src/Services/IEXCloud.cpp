#include "IEXCloud.hpp"

using namespace iex;

void import_existing_data()
{
    
}

IEXCloud::IEXCloud() 
{
    
}

/**
 * @brief 
 * 
 * @param symbol 
 * @return FundOwnership 
 */
std::vector<FundOwnership> IEXCloud::getFundOwnership(String symbol)
{
    String response = client.get_fund_ownership(symbol);
    return parser.parse_fund_ownership(response);
}

#ifndef IEX_Parser_hpp
#define IEX_Parser_hpp

#include "../../core.hpp"
#include "Data/FundOwnership.hpp"

namespace JSONObject = boost::property_tree;

namespace iex
{
    class Parser
    {
    private:
        JSONObject::ptree read_response(std::string response);

    public:
        Parser();

        std::vector<FundOwnership> parse_fund_ownership(std::string response);
    };
}

#endif
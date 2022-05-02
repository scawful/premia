#ifndef IEX_Parser_hpp
#define IEX_Parser_hpp

#include "Boost.hpp"
#include "Data/FundOwnership.hpp"
#include "Data/InsiderTransactions.hpp"

namespace pt = boost::property_tree;

namespace iex
{
    class Parser
    {
    private:
        void log_response(String title, pt::ptree data);
        pt::ptree read_response(String response);

    public:
        Parser();

        std::vector<FundOwnership> parse_fund_ownership(String response);

        std::vector<InsiderTransactions> parse_insider_transactions(String response);
    };
}

#endif
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
        void log_response(std::string title, pt::ptree data);
        pt::ptree read_response(std::string response);

    public:
        Parser();

        std::vector<FundOwnership> parse_fund_ownership(std::string response);

        std::vector<InsiderTransactions> parse_insider_transactions(std::string response);
    };
}

#endif
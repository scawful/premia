#ifndef KuCoin_Parser_hpp
#define KuCoin_Parser_hpp

#include "../../core.hpp"
#include "MarginAccount.hpp"

namespace kc
{
    class Parser
    {
    private:

    public:
        Parser();

        MarginAccount parse_margin_account(boost::property_tree::ptree data);
    };
}

#endif
#include "Parser.hpp"

namespace kc
{
    Parser::Parser()
    {

    }

    MarginAccount Parser::parse_margin_account(boost::property_tree::ptree data) 
    {
        MarginAccount margin_account;
        //    {
        //     "accounts": [
        //     {
        //         "availableBalance": "990.11",
        //         "currency": "USDT",
        //         "holdBalance": "7.22",
        //         "liability": "66.66",
        //         "maxBorrowSize": "88.88",
        //         "totalBalance": "997.33"
        //     }
        //     ],
        //     "debtRatio": "0.33"
        // }
        return margin_account;
    }

}

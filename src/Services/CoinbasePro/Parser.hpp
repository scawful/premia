#ifndef CBP_Parser_hpp
#define CBP_Parser_hpp

#include "Metatypes.hpp"
#include "Library/Boost.hpp"
#include "Account.hpp" // include your objects here 
#include "Product.hpp"

namespace cbp 
{
    struct Transfers {
        float depositsUSD;
    };

    class Parser {
    public:
        json::ptree read_response(CRString response) const;
        Account parse_account_data(const json::ptree & data) const;
        Product parse_product_data(const json::ptree & data) const;
        Transfers parse_transfer_data(const json::ptree & data) const;
    };

}

#endif
#ifndef Product_hpp
#define Product_hpp

#include "../../core.hpp"

namespace cbp
{
    class Product 
    {
    private:
        boost::property_tree::ptree product_data;
        std::vector< std::unordered_map<std::string, std::string > > accounts_vector;

        float current_price;

        void initVariables();

    public:
        Product( boost::property_tree::ptree product_data );

        float get_current_price();


    };
}

#endif
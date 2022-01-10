#include "Product.hpp"

namespace cbp
{
    void Product::initVariables()
    {
        for ( auto & product_it : product_data )
        {
            if ( product_it.first == "price" )
                current_price = product_it.second.get_value<float>();
        }
    }

    Product::Product( boost::property_tree::ptree product_data )
    {
        this->product_data = product_data;
        initVariables();
    }

    float Product::get_current_price()
    {
        return current_price;
    }


}
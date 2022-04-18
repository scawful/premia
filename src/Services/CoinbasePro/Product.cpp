#include "Product.hpp"

namespace cbp
{
    void Product::initVariables()
    {
        for (const auto & [key, value] : product_data)
        {
            if (key == "price")
                current_price = value.get_value<float>();
        }
    }

    Product::Product(const boost::property_tree::ptree & data) : product_data(data)
    {
        initVariables();
    }

    float Product::get_current_price() const
    {
        return current_price;
    }


}
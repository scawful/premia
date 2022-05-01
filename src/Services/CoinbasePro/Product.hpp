#ifndef Product_hpp
#define Product_hpp

#include "Premia.hpp"

namespace cbp
{
    class Product 
    {
    private:
        std::vector<std::unordered_map<std::string, std::string>> accounts_vector;

        float current_price;

        void initVariables();

    public:
        Product()=default;

        float get_current_price() const;


    };
}

#endif
#ifndef CBP_Account_hpp
#define CBP_Account_hpp

#include "Premia.hpp"

namespace cbp
{
    class Account 
    {
    private:
        std::vector<StringMap> accounts_vector;
    public:
        Account()=default;
        StringMap get_position(CRString ticker);
    };
}

#endif
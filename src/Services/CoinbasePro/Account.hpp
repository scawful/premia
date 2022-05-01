#ifndef CBP_Account_hpp
#define CBP_Account_hpp

#include "Premia.hpp"

namespace cbp
{
    class Account 
    {
    private:
        std::vector<std::unordered_map<std::string, std::string >> accounts_vector;
    public:
        Account()=default;
        std::unordered_map<std::string, std::string> get_position(const std::string & ticker);
    };
}

#endif
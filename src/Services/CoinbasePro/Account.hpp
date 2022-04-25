#ifndef CBP_Account_hpp
#define CBP_Account_hpp

#include "core.hpp"

namespace cbp
{
    class Account 
    {
    private:
        boost::property_tree::ptree accountData;
        std::vector<std::unordered_map<std::string, std::string >> accounts_vector;

        void initVariables();

    public:
        explicit Account(const boost::property_tree::ptree & account_data);


        std::unordered_map<std::string, std::string> get_position(const std::string & ticker);

    };
}

#endif
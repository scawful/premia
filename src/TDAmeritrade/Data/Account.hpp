#ifndef Account_hpp
#define Account_hpp

#include "../../core.hpp"
#include "PricingStructures.hpp"

namespace tda
{
    // ================ Account Class Header ================ //

    class Account
    {
    private:
        boost::property_tree::ptree accountData;

        std::unordered_map<std::string, std::string> accountInfoMap;
        std::unordered_map<std::string, std::string> positionsInfoMap;
        
        std::unordered_map<std::string, std::string> currentBalanceMap;        

        void initVariables();

    public:
        Account( boost::property_tree::ptree account_data );

        std::string getAccountVariable( std::string variable );
    };

}

#endif
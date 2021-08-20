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
        int num_positions;

        boost::property_tree::ptree accountData;

        std::unordered_map<std::string, std::string> account_info;
        std::unordered_map<std::string, std::string> current_balances;  
        std::vector<std::unordered_map<std::string, std::string>> positions_vector;

        struct PositionBalances
        {
            std::string symbol;
            std::unordered_map<std::string, std::string> balances;
            int longQuantity;
            int averagePrice;
            double dayProfitLoss;
        };
        std::vector<PositionBalances> position_balances;
        

        void initVariables();

    public:
        Account( boost::property_tree::ptree account_data );

        size_t get_position_vector_size();
        std::string get_account_variable( std::string variable );
        std::string get_balance_variable( std::string variable );
        std::unordered_map<std::string, std::string> get_position( int index );
        std::string get_position_balances( std::string symbol, std::string variable );
    };

}

#endif
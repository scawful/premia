#ifndef Account_hpp
#define Account_hpp

#include "PricingStructures.hpp"

namespace tda
{
    struct PositionBalances
    {
        String symbol;
        StringMap balances;
        int longQuantity;
        int averagePrice;
        double dayProfitLoss;
    };

    class Account
    {
    private:
        int num_positions;

        StringMap account_info;
        StringMap current_balances;  
        std::vector<StringMap> positions_vector;

        std::vector<PositionBalances> position_balances;
        
    public:
        Account()=default;

        void add_position(StringMap position);
        void add_balance(PositionBalances balance);
        void set_account_variable(String key, String value);
        void set_balance_variable(String key, String value);

        size_t get_position_vector_size();
        String get_account_variable( String variable );
        String get_balance_variable( String variable );
        StringMap get_position( int index );
        String get_position_balances( String symbol, String variable );
    };

}

#endif
#ifndef Order_hpp
#define Order_hpp

#include "PricingStructures.hpp"
#include "Boost.hpp"

namespace tda
{
    class Order
    {
    private:
        struct OrderLegCollection {
            int quantity;
            String symbol;
            String assetType;
            String instruction;
        };

        String orderType;
        String price;
        String session;
        String duration;
        String orderStrategyType;
        String complexOrderStrategyType;
        ArrayList<OrderLegCollection> orderLegCollection;

        String orderJSON;

        void prepareOrderString() const;

    public:
        Order();
        ~Order();

        String getString() const;
        String getOrderType() const;
        String getSession() const;
        String getDuration() const;
        String getOrderStrategyType() const;
        String getPrice() const;
    };

    // Buy Market: Stock
    // "orderType": "MARKET",
    // "session": "NORMAL",
    // "duration": "DAY",
    // "orderStrategyType": "SINGLE",
    // "orderLegCollection": [
    //     {
    //     "instruction": "Buy",
    //     "quantity": 15,
    //     "instrument": {
    //         "symbol": "XYZ",
    //         "assetType": "EQUITY"
    //     }
    //     }
    // ]

    // Buy Limit: Single Option
    // "complexOrderStrategyType": "NONE",
    // "orderType": "LIMIT",
    // "session": "NORMAL",
    // "price": "6.45",
    // "duration": "DAY",
    // "orderStrategyType": "SINGLE",
    // "orderLegCollection": [
    //     {
    //     "instruction": "BUY_TO_OPEN",
    //     "quantity": 10,
    //     "instrument": {
    //         "symbol": "XYZ_032015C49",
    //         "assetType": "OPTION"
    //         }
    //     }
    // ]
}

#endif
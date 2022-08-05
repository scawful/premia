#ifndef Order_hpp
#define Order_hpp

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "PricingStructures.hpp"

namespace premia {
namespace tda {
class Order {
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
}  // namespace tda
}  // namespace premia

#endif
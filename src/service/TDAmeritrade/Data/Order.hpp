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
    std::string symbol;
    std::string assetType;
    std::string instruction;
  };

  std::string orderType;
  std::string price;
  std::string session;
  std::string duration;
  std::string orderStrategyType;
  std::string complexOrderStrategyType;
  std::vector<OrderLegCollection> orderLegCollection;

  std::string orderJSON;

  void prepareOrderString() const;

 public:
  Order();
  ~Order();

  std::string getString() const;
  std::string getOrderType() const;
  std::string getSession() const;
  std::string getDuration() const;
  std::string getOrderStrategyType() const;
  std::string getPrice() const;
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
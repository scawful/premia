#include "Order.hpp"
namespace Premia {
namespace tda {

void Order::prepareOrderString() const {
  boost::property_tree::ptree orderPropertyTree;

  orderPropertyTree.put("orderType", getOrderType());
  orderPropertyTree.put("session", getSession());
  orderPropertyTree.put("duration", getDuration());
  orderPropertyTree.put("price", getPrice());
  orderPropertyTree.put("orderStrategyType", getOrderStrategyType());

  try {
    write_json(orderJSON, orderPropertyTree);
  } catch (const boost::property_tree::ptree_error& e) {
    e.what();
  }
}

Order::Order() = default;
Order::~Order() = default;

String Order::getString() const { return orderJSON; }

String Order::getOrderType() const { return orderType; }

String Order::getSession() const { return session; }

String Order::getDuration() const { return duration; }

String Order::getOrderStrategyType() const { return orderStrategyType; }

String Order::getPrice() const { return price; }
}  // namespace tda
}  // namespace Premia
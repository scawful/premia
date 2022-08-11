#include "Order.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "PricingStructures.hpp"

namespace premia {
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

std::string Order::getString() const { return orderJSON; }

std::string Order::getOrderType() const { return orderType; }

std::string Order::getSession() const { return session; }

std::string Order::getDuration() const { return duration; }

std::string Order::getOrderStrategyType() const { return orderStrategyType; }

std::string Order::getPrice() const { return price; }
}  // namespace tda
}  // namespace premia
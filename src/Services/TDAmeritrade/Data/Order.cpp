#include "Order.hpp"
using namespace tda;

void Order::prepareOrderString() const
{
    boost::property_tree::ptree orderPropertyTree;

    orderPropertyTree.add_child("orderType", getOrderType());
    orderPropertyTree.add_child("session", getSession());
    orderPropertyTree.add_child("duration", getDuration());
    orderPropertyTree.add_child("price", getPrice());
    orderPropertyTree.add_child("orderStrategyType", getOrderStrategyType());

    try {
        write_json(orderJSON, orderPropertyTree);
    } catch (const boost::property_tree::ptree_error & e) {
        e.what();
    }
}

Order::Order()=default;
Order::~Order()=default;

std::string Order::getString() const {
    return orderJSON;
}

std::string Order::getOrderType() const {
    return orderType;
}

std::string Order::getSession() const {
    return session;
}

std::string Order::getDuration() const {
    return duration;
}

std::string Order::getOrderStrategyType() const {
    return orderStrategyType;
}

std::string Order::getPrice() const {
    return price;
}
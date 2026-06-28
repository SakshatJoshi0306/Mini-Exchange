#pragma once

#include <deque>

#include "Order.hpp"

class PriceLevel
{
public:

    explicit PriceLevel(long price_);

    long getPriceTicks() const;

    void addOrder(const Order& order);

    Order& frontOrder();

    void removeFrontOrder();

    bool removeOrder(int orderID);

    bool empty() const;

    size_t numberOfOrders() const;

    int totalQuantity() const;


private:

    long priceTicks;

    std::deque<Order> orders_;
};
#pragma once

#include <deque>

#include "Order.hpp"

class PriceLevel
{
public:

    explicit PriceLevel(long price_);

    long getPriceTicks() const;

    void addOrder(const Order& order);

    const Order& frontOrder() const;

    void removeFrontOrder();

    bool empty() const;

    size_t numberOfOrders() const;

    int totalQuantity() const;

private:

    long priceTicks;

    std::deque<Order> orders_;
};
#pragma once

#include <deque>
#include <optional>

#include "order.hpp"

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

    std::optional<Order> findOrder(int orderID) const;
    
private:

    long priceTicks;

    std::deque<Order> orders_;
};
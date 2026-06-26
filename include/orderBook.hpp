#pragma once

#include <map>
#include <functional>

#include "Order.hpp"
#include "PriceLevel.hpp"

class OrderBook
{
public:

    void addOrder(const Order& order);

    void printBook() const;

    void printBids() const;

    void printAsks() const;

    bool empty() const;

    long getBestBid() const;

    long getBestAsk() const;

private:

    std::map<long, PriceLevel, std::greater<long>> bidBook_;

    std::map<long, PriceLevel> askBook_;
};

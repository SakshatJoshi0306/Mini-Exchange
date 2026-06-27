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

    bool hasBids() const;

    bool hasAsks() const;

    // Returns the best (lowest) ask price level.
// We expose only the PriceLevel, not the underlying map,
// so the MatchingEngine cannot accidentally manipulate the map itself.
    PriceLevel& bestAsk();

// Returns the best (highest) bid price level.
// Same reasoning as above.
    PriceLevel& bestBid();

// Removes the current best ask level.
// Used after every order at that price has been completely filled.
    void removeBestAskLevel();

// Removes the current best bid level.
// Symmetric to removeBestAskLevel().
    void removeBestBidLevel();

private:

    std::map<long, PriceLevel, std::greater<long>> bidBook_; //sorted in descending order so first element is the best bid

    std::map<long, PriceLevel> askBook_;
};

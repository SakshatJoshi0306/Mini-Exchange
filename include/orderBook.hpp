#pragma once

#include <map>
#include <functional>
#include <unordered_map>
#include <optional>

#include "order.hpp"
#include "PriceLevel.hpp"
struct OrderLocation
{
    Side side;
    long price;
};
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

    bool cancelOrder(int orderID);

    int restingOrders() const;

    int priceLevelCount() const;

    double averageQueueLength() const;

//=============================================================
// Read-only access to the bid book.
// Used by MatchingEngine for liquidity checks (e.g. FOK).
//=============================================================

const std::map<long, PriceLevel, std::greater<long>>& getBidBook() const;

//=============================================================
// Read-only access to the ask book.
//=============================================================

const std::map<long, PriceLevel>& getAskBook() const;

    std::optional<Order> findOrder(int orderID) const;

private:

    std::map<long, PriceLevel, std::greater<long>> bidBook_; //sorted in descending order so first element is the best bid

    std::map<long, PriceLevel> askBook_;

    std::unordered_map<int, OrderLocation> orderIndex_;
};

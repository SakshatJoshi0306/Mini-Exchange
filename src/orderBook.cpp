#include "orderBook.hpp"

#include<iostream>
#include <iomanip>

void OrderBook::addOrder(const Order& order)
{
    if(order.getSide() == Side::BUY)
    {
        long price = order.getPriceTicks();

        auto it = bidBook_.find(price);

        if (it == bidBook_.end())
        {
            PriceLevel level(price);
            level.addOrder(order);

            bidBook_.insert({price, level});
        }
        else
        {
            it->second.addOrder(order); //first points to the key of a map, second points to the value, so we just append the new order to the deque
        }
        orderIndex_[order.getId()] ={Side::BUY, order.getPriceTicks()};
    }
    else
    {
        long price = order.getPriceTicks();

        auto it = askBook_.find(price);

        if (it == askBook_.end())
        {
            PriceLevel level(price);
            level.addOrder(order);

            askBook_.insert({price, level});
        }
        else
        {
            it->second.addOrder(order); 
        }
        orderIndex_[order.getId()] ={Side::SELL, order.getPriceTicks()};
    }
}

bool OrderBook::empty() const
{
    return bidBook_.empty() && askBook_.empty();
}

long OrderBook::getBestBid() const
{
    if (bidBook_.empty())
        return -1;

    return bidBook_.begin()->first; //simply returning the key of the map(the pricelevel) of the maps top/largest element since its the highest bid
} 

long OrderBook::getBestAsk() const
{
    if (askBook_.empty())
        return -1;

    return askBook_.begin()->first; 
}

void OrderBook::printBids() const
{
    std::cout << "\n================== BID SIDE ==================\n\n";

    std::cout
        << std::left
        << std::setw(12) << "Price"
        << std::setw(12) << "Orders"
        << std::setw(12) << "Quantity"
        << '\n';

    std::cout
        << "----------------------------------------------\n";

    for (const auto& [price, level] : bidBook_)
    {
        std::cout
            << std::setw(12) << price
            << std::setw(12) << level.numberOfOrders()
            << std::setw(12) << level.totalQuantity()
            << '\n';
    }
}

void OrderBook::printAsks() const
{
    std::cout << "\n================== ASK SIDE ==================\n\n";

    std::cout
        << std::left
        << std::setw(12) << "Price"
        << std::setw(12) << "Orders"
        << std::setw(12) << "Quantity"
        << '\n';

    std::cout
        << "----------------------------------------------\n";

    for (const auto& [price, level] : askBook_)
    {
        std::cout
            << std::setw(12) << price
            << std::setw(12) << level.numberOfOrders()
            << std::setw(12) << level.totalQuantity()
            << '\n';
    }
}

void OrderBook::printBook() const
{
    std::cout
        << "\n\n=========================================================\n"
        << "                 LEVEL II ORDER BOOK\n"
        << "=========================================================\n";

    printAsks();

    std::cout
        << "\n=========================================================\n";

    printBids();

    std::cout
        << "\n=========================================================\n";
}

bool OrderBook::hasBids() const
{
    return !bidBook_.empty();
}

bool OrderBook::hasAsks() const
{
    return !askBook_.empty();
}

PriceLevel& OrderBook::bestAsk()
{
    // begin() always points to the lowest ask because
    // askBook_ is sorted in ascending order.
    return askBook_.begin()->second;
}

PriceLevel& OrderBook::bestBid()
{
    // begin() always points to the highest bid because
    // bidBook_ uses std::greater<long>.
    return bidBook_.begin()->second;
}

void OrderBook::removeBestAskLevel()
{
    // erase(begin()) removes the lowest ask level.
    // We call this only after its queue becomes empty.
    askBook_.erase(askBook_.begin());
}

void OrderBook::removeBestBidLevel()
{
    // Same logic for bids.
    bidBook_.erase(bidBook_.begin());
}
bool OrderBook::cancelOrder(int orderID)
{
    // ---------- Step 1 ----------
    // Check if the order exists.
    auto indexIt = orderIndex_.find(orderID);

    if (indexIt == orderIndex_.end())
    {
        return false;
    }

    // ---------- Step 2 ----------
    // Get the price level from the hash map.
    Side side = indexIt->second.side;
    long price = indexIt->second.price;

    // ---------- Step 3 ----------
    // Search the bid side first.
    auto bidLevel = bidBook_.find(price);

    if (bidLevel != bidBook_.end())
    {
        if (bidLevel->second.removeOrder(orderID))
        {
            if (bidLevel->second.empty())
                bidBook_.erase(bidLevel);

            orderIndex_.erase(orderID);

            return true;
        }
    }

    // ---------- Step 4 ----------
    // Otherwise search the ask side.
    auto askLevel = askBook_.find(price);

    if (askLevel != askBook_.end())
    {
        if (askLevel->second.removeOrder(orderID))
        {
            if (askLevel->second.empty())
                askBook_.erase(askLevel);

            orderIndex_.erase(orderID);

            return true;
        }
    }

    return false;
}

std::optional<Order> OrderBook::findOrder(int orderID) const
{
    // Look up the order's location using its ID.
    auto indexIt = orderIndex_.find(orderID);

    // Order doesn't exist.
    if (indexIt == orderIndex_.end())
        return std::nullopt;

    // Retrieve the stored location.
    Side side = indexIt->second.side;
    long price = indexIt->second.price;

    // Go directly to the correct side of the book.
    if (side == Side::BUY)
    {
        auto bidLevel = bidBook_.find(price);

        if (bidLevel == bidBook_.end())
            return std::nullopt;

        return bidLevel->second.findOrder(orderID);
    }
    else
    {
        auto askLevel = askBook_.find(price);

        if (askLevel == askBook_.end())
            return std::nullopt;

        return askLevel->second.findOrder(orderID);
    }
}

const std::map<long, PriceLevel, std::greater<long>>& OrderBook::getBidBook() const
{
    return bidBook_;
}

const std::map<long, PriceLevel>& OrderBook::getAskBook() const
{
    return askBook_;
}

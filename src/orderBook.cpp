#include "OrderBook.hpp"

#include<iostream>

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
    std::cout << "\n------ BID BOOK ------\n";

    for (const auto& [price, level] : bidBook_)
    {
        std::cout
            << price
            << " | "
            << level.numberOfOrders()
            << " orders | Qty "
            << level.totalQuantity()
            << '\n';
    }
}

void OrderBook::printAsks() const
{
    std::cout << "\n------ ASK BOOK ------\n";

    for (const auto& [price, level] : askBook_)
    {
        std::cout
            << price
            << " | "
            << level.numberOfOrders()
            << " orders | Qty "
            << level.totalQuantity()
            << '\n';
    }
}

void OrderBook::printBook() const
{
    std::cout << "\n==============================\n";

    printAsks();

    std::cout << "------------------------------\n";

    printBids();

    std::cout << "==============================\n";
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
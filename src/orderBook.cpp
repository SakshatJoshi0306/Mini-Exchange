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
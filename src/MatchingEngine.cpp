#include "MatchingEngine.hpp"
#include <iostream>

void MatchingEngine::ProcessOrder(Order order) 
{
    if(order.getSide() == Side::BUY)
    {
        matchBuyOrder(order);
    }
    else
    {
        matchSellOrder(order);
    }
}
void MatchingEngine::matchBuyOrder(Order& incoming)
{
    // Continue matching until:
    // 1. Incoming order is completely filled.
    // 2. No asks remain.
    // 3. Prices no longer cross.
    while (!incoming.isFilled() && orderBook_.hasAsks())
    {
        // Lowest sell price currently available.
        long bestAsk = orderBook_.getBestAsk();

        // Buy limit price is below the best ask.
        // Cannot trade any further. if ordertype is market it will go on until incoming are filled or until the orderbook has asks
        if (incoming.getOrderType() == OrderType::LIMIT && incoming.getPriceTicks() < bestAsk)
        {
            break;
        }

        // Access the best price level.
        PriceLevel& level = orderBook_.bestAsk();

        // Oldest order at this price (FIFO).
        Order& resting = level.frontOrder();

        // Trade the smaller remaining quantity.
        int tradedQuantity = std::min(
            incoming.getQuantity(),
            resting.getQuantity()
        );

        // Reduce both orders.
        incoming.reduceQuantity(tradedQuantity);
        resting.reduceQuantity(tradedQuantity);

        // Remove the resting order if it has been fully executed.
        if (resting.isFilled())
        {
            level.removeFrontOrder();
        }

        // Remove the price level if no orders remain at that price.
        if (level.empty())
        {
            orderBook_.removeBestAskLevel();
        }
    }

    // If some quantity remains after matching,
    // it becomes a resting limit order.
    if (!incoming.isFilled())
    {
        orderBook_.addOrder(incoming);
    }
}
void MatchingEngine::matchSellOrder(Order& incoming)
{
    // Continue matching while:
    // 1. Incoming sell still has quantity.
    // 2. Buyers exist in the book.
    while (!incoming.isFilled() && orderBook_.hasBids())
    {
        // Highest buy price currently available.
        long bestBid = orderBook_.getBestBid();

        // Seller wants more than the highest buyer is willing to pay.
        // Prices do not cross, so matching stops.
        if (incoming.getOrderType() == OrderType::LIMIT && incoming.getPriceTicks() > bestBid)
            break;

        // Highest bid price level.
        PriceLevel& level = orderBook_.bestBid();

        // Oldest order at that price (FIFO).
        Order& resting = level.frontOrder();

        // Execute the maximum possible trade.
        int tradedQuantity = std::min(
            incoming.getQuantity(),
            resting.getQuantity()
        );

        // Reduce remaining quantities.
        incoming.reduceQuantity(tradedQuantity);
        resting.reduceQuantity(tradedQuantity);

        // Remove fully executed resting order.
        if (resting.isFilled())
        {
            level.removeFrontOrder();
        }

        // Remove the entire price level if no orders remain.
        if (level.empty())
        {
            orderBook_.removeBestBidLevel();
        }
    }

    // If some quantity remains after matching,
    // it becomes a resting sell order.
    if (!incoming.isFilled())
    {
        orderBook_.addOrder(incoming);
    }
}
void MatchingEngine::executeTrade(Order& incoming, Order& resting, long quantity, long priceTicks)
{
    
}
const OrderBook& MatchingEngine::getOrderBook() const
{
    return orderBook_;
}

bool MatchingEngine::cancelOrder(int orderID)
{
    return orderBook_.cancelOrder(orderID);
}
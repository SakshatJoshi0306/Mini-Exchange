#pragma once
#include "OrderBook.hpp"
#include "Order.hpp"

class MatchingEngine
{
    public:
        void ProcessOrder(Order order);
        const OrderBook& getOrderBook() const;
        bool cancelOrder(int orderID);
        
    private:
        OrderBook orderBook_;

        void matchBuyOrder(Order& order);

        void matchSellOrder(Order& order);

        void executeTrade(
            Order& incoming,
            Order& resting,
            long quantity,
            long priceTicks
        );

        
};
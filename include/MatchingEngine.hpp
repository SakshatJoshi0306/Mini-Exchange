#pragma once
#include "orderBook.hpp"
#include "order.hpp"

class MatchingEngine
{
    public:
        void ProcessOrder(Order order);
        const OrderBook& getOrderBook() const;
        bool cancelOrder(int orderID);
        bool modifyOrder(int orderID, long newPrice, int newQuantity);
        
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
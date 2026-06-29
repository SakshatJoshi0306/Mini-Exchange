#pragma once
#include "orderBook.hpp"
#include "order.hpp"
#include "Trade.hpp"
#include <vector>

class MatchingEngine
{
    public:
        void ProcessOrder(Order order);
        const OrderBook& getOrderBook() const;
        bool cancelOrder(int orderID);
        bool modifyOrder(int orderID, long newPrice, int newQuantity);

        void recordTrade(const Order& resting, const Order& incoming, int tradedQuantity, long executionPrice);

        void printTradeHistory() const;
        
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

        std::vector<Trade> tradeHistory_;

        
};
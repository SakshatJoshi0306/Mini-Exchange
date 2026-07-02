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

        int tradeCount() const;

        int totalVolume() const;

        long lastTradedPrice() const;

        long highestTradePrice() const;

        long lowestTradePrice() const;

        double calculateVWAP() const; //VWAP stands for volume weighted average price

        void printMarketStatistics() const;
        
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

        bool canFullyFill(const Order& order) const;

        std::vector<Trade> tradeHistory_;

        std::vector<Order> stopOrders_;

        void checkStopOrders(long lastTradePrice);

        void triggerStopOrder(std::size_t index);

        
};
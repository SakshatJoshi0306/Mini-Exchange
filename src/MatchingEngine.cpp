#include "MatchingEngine.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>


void MatchingEngine::ProcessOrder(Order order) 
{
    if (order.isStopOrder())
    {
        stopOrders_.push_back(order);
        return;
    }
    // FIRST: FOK liquidity check
    if (order.getOrderType() == OrderType::FOK)
    {
        if (!canFullyFill(order))
        {
            return;
        }
    }
    if(order.getSide() == Side::BUY)
    {
        matchBuyOrder(order);
    }
    else
    {
        matchSellOrder(order);
    }
    if (!order.isFilled() && order.getOrderType() == OrderType::LIMIT)
    {
        order.initializeVisibleSlice();
        orderBook_.addOrder(order);
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

        if (incoming.getClientID() == resting.getClientID())
        {
            std::cout << "Self-match prevented (Cancel Newest policy). \n Incoming order cancelled.\n";
            return;
        }

        // Trade the smaller remaining quantity.
        int tradedQuantity = std::min(
            incoming.getRemainingQuantity(),
            resting.getVisibleQuantity()
        );

        // Reduce both orders.
        incoming.reduceQuantity(tradedQuantity);
        resting.reduceQuantity(tradedQuantity);

        recordTrade(incoming, resting, tradedQuantity, resting.getPriceTicks());

        if (resting.getVisibleQuantity() == 0)
        {
            level.processFrontAfterExecution();
        }

        // Remove the price level if no orders remain at that price.
        if (level.empty())
        {
            orderBook_.removeBestAskLevel();
        }
    }

    // If some quantity remains after matching,
    // it becomes a resting limit order.
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

        if (incoming.getClientID() == resting.getClientID())
        {
            std::cout << "Self-match prevented. Incoming order cancelled.\n";
            return;
        }

        // Execute the maximum possible trade.
        int tradedQuantity = std::min(
            incoming.getRemainingQuantity(),
            resting.getVisibleQuantity()
        );

        // Reduce remaining quantities.
        incoming.reduceQuantity(tradedQuantity);
        resting.reduceQuantity(tradedQuantity);

        recordTrade(incoming, resting, tradedQuantity, resting.getPriceTicks());

        if (resting.getVisibleQuantity() == 0)
        {
            level.processFrontAfterExecution();
        }
        // Remove the entire price level if no orders remain.
        if (level.empty())
        {
            orderBook_.removeBestBidLevel();
        }
    }

    // If some quantity remains after matching,
    // it becomes a resting sell order.
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

bool MatchingEngine::modifyOrder(int orderID,
                                 long newPrice,
                                 int newQuantity)
{
    // Look up the current resting order.
    auto order = orderBook_.findOrder(orderID);

    if (!order.has_value())
        return false;

    // Remove the old order from the book.
    orderBook_.cancelOrder(orderID);

    // Construct the replacement order.
    // It receives a new price and/or quantity but keeps
    // the same ID, side, and order type.
    Order modifiedOrder(
        orderID,
        order->getClientID(),
        order->getSide(),
        newQuantity,
        newPrice,
        order->getOrderType()
    );

    // Treat it exactly like a brand-new incoming order.
    // This guarantees matching logic is reused and
    // time priority is reset.
    ProcessOrder(modifiedOrder);

    return true;
}

void MatchingEngine::recordTrade(const Order& incoming, const Order& resting, int tradedQuantity,  long executionPrice)                                
{
    int buyID;
    int sellID;

    // Determine which order is the buyer.
    if (incoming.getSide() == Side::BUY)
    {
        buyID = incoming.getId();
        sellID = resting.getId();
    }
    else
    {
        buyID = resting.getId();
        sellID = incoming.getId();
    }

    //=========================================================
    // emplace_back constructs the Trade directly inside the
    // vector.
    //
    // No temporary Trade object is created.
    //=========================================================

    tradeHistory_.emplace_back(
        buyID,
        sellID,
        executionPrice,
        tradedQuantity
    );

//---------------------------------------------------------
// Update cached statistics.
//---------------------------------------------------------

        tradeCount_++;

        totalVolume_ += tradedQuantity;

        lastTradePrice_ = executionPrice;

        if (tradeCount_ == 1)
        {
            highestTradePrice_ = executionPrice;
            lowestTradePrice_ = executionPrice;
        }
        else
        {
            highestTradePrice_ = std::max(highestTradePrice_, executionPrice);

            lowestTradePrice_ = std::min(lowestTradePrice_, executionPrice);
        }

cumulativeTradeValue_ += static_cast<long long>(executionPrice)*tradedQuantity;

    checkStopOrders(executionPrice);
}

void MatchingEngine::printTradeHistory() const
{
    std::cout << "\n================ TIME & SALES ================\n\n";

    std::cout
        << std::left
        << std::setw(12) << "Time"
        << std::setw(10) << "Price"
        << std::setw(10) << "Qty"
        << std::setw(10) << "Buyer"
        << std::setw(10) << "Seller"
        << '\n';

    std::cout
        << "----------------------------------------------------------\n";

    for (const auto& trade : tradeHistory_)
    {
        std::time_t time =
            std::chrono::system_clock::to_time_t(trade.timestamp);

        std::cout
            << std::setw(12)
            << std::put_time(std::localtime(&time), "%H:%M:%S")

            << std::setw(10)
            << trade.price

            << std::setw(10)
            << trade.quantity

            << std::setw(10)
            << trade.buyOrderID

            << std::setw(10)
            << trade.sellOrderID

            << '\n';
    }

    std::cout
        << "\n==============================================\n";
}

int MatchingEngine::tradeCount() const
{
    // vector already knows its size.

    return tradeCount_;
}

int MatchingEngine::totalVolume() const
{
    return totalVolume_;
}

long MatchingEngine::lastTradedPrice() const
{
    return lastTradePrice_;
}

long MatchingEngine::highestTradePrice() const
{
    return highestTradePrice_;
}

long MatchingEngine::lowestTradePrice() const
{
    return lowestTradePrice_;
}

double MatchingEngine::calculateVWAP() const
{
    if(totalVolume_ == 0) return 0.0;

    return static_cast<double>(cumulativeTradeValue_) / totalVolume_;
}

void MatchingEngine::printMarketStatistics() const
{
    std::cout << "\n========== MARKET STATISTICS ==========\n";

    std::cout << "Trades            : "
              << tradeCount()
              << '\n';

    std::cout << "Total Volume      : "
              << totalVolume()
              << '\n';

    std::cout << "Last Trade Price  : "
              << lastTradedPrice()
              << '\n';

    std::cout << "Highest Trade     : "
              << highestTradePrice()
              << '\n';

    std::cout << "Lowest Trade      : "
              << lowestTradePrice()
              << '\n';

    std::cout << "VWAP              : "
              << std::fixed
              << std::setprecision(2)
              << calculateVWAP()
              << '\n';

    std::cout << "=======================================\n";
}

bool MatchingEngine::canFullyFill(const Order& order) const
{
    int remaining = order.getQuantity();

    if (order.getSide() == Side::BUY)
    {
        // Walk through asks.

        for (const auto& [price, level] : orderBook_.getAskBook())
        {
            // Price no longer crosses.
            if (order.getOrderType() != OrderType::MARKET &&
                price > order.getPriceTicks())
            {
                break;
            }

            remaining -= level.totalQuantity();

            if (remaining <= 0)
                return true;
        }
    }
    else
    {
        // Walk through bids.

        for (const auto& [price, level] : orderBook_.getBidBook())
        {
            if (order.getOrderType() != OrderType::MARKET &&
                price < order.getPriceTicks())
            {
                break;
            }

            remaining -= level.totalQuantity();

            if (remaining <= 0)
                return true;
        }
    }

    return false;
}

void MatchingEngine::checkStopOrders(long lastTradePrice)
{
    std::size_t i = 0;

    while (i < stopOrders_.size())
    {
        const Order& order = stopOrders_[i];

        bool trigger = false;

        if (order.getSide() == Side::BUY)
        {
            trigger = lastTradePrice >= order.getStopPrice();
        }
        else
        {
            trigger = lastTradePrice <= order.getStopPrice();
        }

        if (trigger)
        {
            triggerStopOrder(i);
        }
        else
        {
            ++i;
        }
    }
}

void MatchingEngine::triggerStopOrder(std::size_t index)
{
    Order order = stopOrders_[index];

    order.activateStop();

    stopOrders_.erase(stopOrders_.begin() + index);

    ProcessOrder(order);
}

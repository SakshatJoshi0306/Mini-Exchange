#include "MatchingEngine.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>


void MatchingEngine::ProcessOrder(Order order) 
{
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

        // Trade the smaller remaining quantity.
        int tradedQuantity = std::min(
            incoming.getQuantity(),
            resting.getQuantity()
        );

        // Reduce both orders.
        incoming.reduceQuantity(tradedQuantity);
        resting.reduceQuantity(tradedQuantity);

        recordTrade(incoming, resting, tradedQuantity, resting.getPriceTicks());

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

        recordTrade(incoming, resting, tradedQuantity, resting.getPriceTicks());

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

    return tradeHistory_.size();
}

int MatchingEngine::totalVolume() const
{
    int volume = 0;

    for (const auto& trade : tradeHistory_)
    {
        volume += trade.quantity;
    }

    return volume;
}

long MatchingEngine::lastTradedPrice() const
{
    if (tradeHistory_.empty())
        return 0;

    return tradeHistory_.back().price;
}

long MatchingEngine::highestTradePrice() const
{
    if (tradeHistory_.empty())
        return 0;

    long highest = tradeHistory_.front().price;

    for (const auto& trade : tradeHistory_)
    {
        if (trade.price > highest)
        {
            highest = trade.price;
        }
    }

    return highest;
}

long MatchingEngine::lowestTradePrice() const
{
    if (tradeHistory_.empty())
        return 0;

    long lowest = tradeHistory_.front().price;

    for (const auto& trade : tradeHistory_)
    {
        if (trade.price < lowest)
        {
            lowest = trade.price;
        }
    }

    return lowest;
}

double MatchingEngine::calculateVWAP() const
{
    if (tradeHistory_.empty())
        return 0.0;

    long long totalValue = 0;
    int totalQty = 0;

    for (const auto& trade : tradeHistory_)
    {
        // Dollar value traded.

        totalValue +=
            static_cast<long long>(trade.price) *
            trade.quantity;

        totalQty += trade.quantity;
    }

    return static_cast<double>(totalValue)
           / totalQty;
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
#include "order.hpp"
#include <stdexcept>
#include <algorithm>
//Initialising constructor for the normal case -> lmit/market
Order::Order(
    int id,
    int clientID,
    Side side,
    int quantity,
    long priceTicks,
    OrderType orderType)
    :
    id(id),
    clientID(clientID),
    side(side),
    quantity(quantity),
    visibleQuantity(quantity),
    peakSize(quantity),
    iceberg(false),
    priceTicks(priceTicks),
    orderType(orderType),
    stopOrder(false),
    stopPrice(0),
    stopMarket(false)
{
}

//Initialising constructor for the iceberg orders
Order::Order(
    int id,
    int clientID,
    Side side,
    int quantity,
    long priceTicks,
    OrderType orderType,
    int peakSize)
    :
    id(id),
    clientID(clientID),
    side(side),
    quantity(quantity),
    visibleQuantity(std::min(quantity, peakSize)),
    peakSize(peakSize),
    iceberg(true),
    priceTicks(priceTicks),
    orderType(orderType),
    stopOrder(false),
    stopPrice(0),
    stopMarket(false)
{
}

//initialising constructor for stop orders -> ordertype after trigger price reaches is already decided in the initialisation
Order::Order(
    int id,
    int clientID,
    Side side,
    int quantity,
    long triggerPrice,
    bool marketAfterTrigger,
    long limitPrice)
    :
    id(id),
    clientID(clientID),
    side(side),
    quantity(quantity),
    visibleQuantity(quantity),
    peakSize(quantity),
    iceberg(false),
    stopOrder(true),
    stopPrice(triggerPrice),
    stopMarket(marketAfterTrigger),
    priceTicks(limitPrice),
    orderType(
        marketAfterTrigger ?
        OrderType::MARKET :
        OrderType::LIMIT)
{
}

int Order::getId() const
{
    return id;
}

int Order::getClientID() const
{
    return clientID;
}

Side Order::getSide() const
{
    return side;
}

int Order::getQuantity() const
{
    return quantity;
}

long Order::getPriceTicks() const
{
    return priceTicks;
}

OrderType Order::getOrderType() const
{
    return orderType;
}

void Order::reduceQuantity(int amount)
{
    if (amount > visibleQuantity)
    {
        throw std::runtime_error(
            "Trade quantity exceeds visible quantity."
        );
    }

    visibleQuantity -= amount;
    quantity -= amount;
}

bool Order::isFilled() const
{
    return quantity == 0;
}

int Order::getVisibleQuantity() const
{
    return visibleQuantity;
}

int Order::getRemainingQuantity() const
{
    return quantity;
}

int Order::getPeakSize() const
{
    return peakSize;
}

bool Order::isIceberg() const
{
    return iceberg;
}

bool Order::needsRefresh() const
{
    return iceberg &&
           visibleQuantity == 0 &&
           quantity > 0;
}

void Order::refreshVisible()
{
    if (!needsRefresh())
        return;

    visibleQuantity = std::min(quantity, peakSize);
}

bool Order::refreshIfNeeded()
{
    if (!needsRefresh())
    {
        return false;
    }

    visibleQuantity = std::min(quantity, peakSize);

    return true;
}

void Order::initializeVisibleSlice()
{
    if (!iceberg)
        return;

    visibleQuantity = std::min(quantity, peakSize);
}

bool Order::isStopOrder() const
{
    return stopOrder;
}

long Order::getStopPrice() const
{
    return stopPrice;
}

bool Order::isStopMarket() const
{
    return stopMarket;
}

void Order::activateStop()
{
    stopOrder = false;
}
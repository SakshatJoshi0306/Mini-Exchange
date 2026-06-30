#include "order.hpp"
#include <stdexcept>
#include <algorithm>

Order::Order(
    int id,
    Side side,
    int quantity,
    long priceTicks,
    OrderType orderType)
    :
    id(id),
    side(side),
    quantity(quantity),
    visibleQuantity(quantity),
    peakSize(quantity),
    iceberg(false),
    priceTicks(priceTicks),
    orderType(orderType)
{
}
Order::Order(
    int id,
    Side side,
    int quantity,
    long priceTicks,
    OrderType orderType,
    int peakSize)
    :
    id(id),
    side(side),
    quantity(quantity),
    visibleQuantity(std::min(quantity, peakSize)),
    peakSize(peakSize),
    iceberg(true),
    priceTicks(priceTicks),
    orderType(orderType)
{
}

int Order::getId() const
{
    return id;
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
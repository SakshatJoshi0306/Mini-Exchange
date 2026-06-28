#include "order.hpp"
#include <stdexcept>

Order::Order(int id,
             Side side,
             int quantity,
             long priceTicks,
             OrderType orderType)
    : id(id),
      side(side),
      quantity(quantity),
      priceTicks(priceTicks),
      orderType(orderType)    // Use the parameter, don't hardcode LIMIT.
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
    // Prevent an order from becoming negative.
    if (amount > quantity)
    {
        throw std::runtime_error("Trade quantity exceeds remaining quantity.");
    }

    quantity -= amount;
}

bool Order::isFilled() const
{
    return quantity == 0;
}
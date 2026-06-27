#include "Order.hpp"
#include<stdexcept>

Order::Order(int id, Side side, int quantity, long priceTicks)
    : id(id), side(side), quantity(quantity), priceTicks(priceTicks) // called an initialiser list
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
void Order::reduceQuantity(int amount)
{
    // Defensive programming.
    // An order should never lose more quantity than it currently has.
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
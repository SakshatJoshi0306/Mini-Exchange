#include "Order.hpp"

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
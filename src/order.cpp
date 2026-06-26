#include "Order.hpp"

Order::Order(int id, Side side, int quantity, double price)
    : id(id), side(side), quantity(quantity), price(price)
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

double Order::getPrice() const
{
    return price;
}
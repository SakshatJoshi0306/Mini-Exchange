#include <iostream>

#include "Order.hpp"
#include "OrderBook.hpp"

int main()
{
    OrderBook book;

    book.addOrder(Order(1, Side::BUY, 100, 25000));
    book.addOrder(Order(2, Side::BUY, 50, 25000));
    book.addOrder(Order(3, Side::SELL, 75, 25100));
    book.addOrder(Order(4, Side::SELL, 20, 25200));

    book.printBook();

    return 0;
}
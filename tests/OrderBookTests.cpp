#include <catch2/catch_test_macros.hpp>

#include "OrderBook.hpp"

TEST_CASE("New OrderBook is empty")
{
    OrderBook book;

    REQUIRE(book.empty());
}

TEST_CASE("Adding orders makes OrderBook non-empty")
{
    OrderBook book;

    book.addOrder(Order(1, 1, Side::BUY, 100, 25000));

    REQUIRE_FALSE(book.empty());
}

TEST_CASE("Best bid is highest buy price")
{
    OrderBook book;

    book.addOrder(Order(1, 1, Side::BUY, 100, 25000));
    book.addOrder(Order(2, 2, Side::BUY, 100, 25200));
    book.addOrder(Order(3, 3, Side::BUY, 100, 25100));

    REQUIRE(book.getBestBid() == 25200);
}

TEST_CASE("Best ask is lowest sell price")
{
    OrderBook book;

    book.addOrder(Order(1, 1, Side::SELL, 100, 25500));
    book.addOrder(Order(2, 2,  Side::SELL, 100, 25300));
    book.addOrder(Order(3, 3, Side::SELL, 100, 25400));

    REQUIRE(book.getBestAsk() == 25300);
}
#include <catch2/catch_test_macros.hpp>

#include "PriceLevel.hpp"

TEST_CASE("New PriceLevel is empty")
{
    PriceLevel level(25000);

    REQUIRE(level.empty());
    REQUIRE(level.numberOfOrders() == 0);
    REQUIRE(level.totalQuantity() == 0);
}

TEST_CASE("Adding one order")
{
    PriceLevel level(25000);

    Order order(1, Side::BUY, 100, 25000);

    level.addOrder(order);

    REQUIRE_FALSE(level.empty());
    REQUIRE(level.numberOfOrders() == 1);
    REQUIRE(level.totalQuantity() == 100);
    REQUIRE(level.frontOrder().getId() == 1);
}

TEST_CASE("FIFO order is maintained")
{
    PriceLevel level(25000);

    level.addOrder(Order(1, Side::BUY, 100, 25000));
    level.addOrder(Order(2, Side::BUY, 50, 25000));

    REQUIRE(level.frontOrder().getId() == 1);

    level.removeFrontOrder();

    REQUIRE(level.frontOrder().getId() == 2);
}

TEST_CASE("Total quantity is summed correctly")
{
    PriceLevel level(25000);

    level.addOrder(Order(1, Side::BUY, 100, 25000));
    level.addOrder(Order(2, Side::BUY, 50, 25000));
    level.addOrder(Order(3, Side::BUY, 25, 25000));

    REQUIRE(level.totalQuantity() == 175);
}
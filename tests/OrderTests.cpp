#include <catch2/catch_test_macros.hpp>

#include "Order.hpp"

TEST_CASE("Order constructor stores values correctly")
{
    Order order(1, 1, Side::BUY, 100, 25000);

    REQUIRE(order.getId() == 1);
    REQUIRE(order.getSide() == Side::BUY);
    REQUIRE(order.getQuantity() == 100);
    REQUIRE(order.getPriceTicks() == 25000);
}
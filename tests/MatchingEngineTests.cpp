#include <catch2/catch_test_macros.hpp>

#include "MatchingEngine.hpp"

TEST_CASE("Buy order enters empty book")
{
    MatchingEngine engine;

    engine.ProcessOrder(
        Order(1, 1, Side::BUY, 100, 25000)
    );

    REQUIRE(engine.getOrderBook().hasBids());
}
TEST_CASE("Buy order below best ask becomes resting order")
{
    MatchingEngine engine;

    engine.ProcessOrder(
        Order(1, 1, Side::SELL, 100, 25010)
    );

    engine.ProcessOrder(
        Order(2, 2, Side::BUY, 100, 25000)
    );

    // Prices do not cross.
    // The buy order should simply enter the bid book.
    REQUIRE(engine.getOrderBook().hasBids());
    REQUIRE(engine.getOrderBook().hasAsks());
}
TEST_CASE("Buy order trades with best ask")
{
    MatchingEngine engine;

    engine.ProcessOrder(
        Order(1, 1, Side::SELL, 100, 25000)
    );

    engine.ProcessOrder(
        Order(2, 2, Side::BUY, 100, 25000)
    );

    // The sell order should be completely filled,
    // so the ask side should now be empty.
    REQUIRE_FALSE(engine.getOrderBook().hasAsks());

    // The incoming buy was also completely filled,
    // so nothing should remain on the bid side.
    REQUIRE_FALSE(engine.getOrderBook().hasBids());
}
#pragma once
#include <chrono>

// Represents ONE completed trade.
//
// Unlike an Order (which may remain on the book),
// a Trade is permanent once created.

struct Trade
{
    int buyOrderID;
    int sellOrderID;

    long price;
    int quantity;

    std::chrono::system_clock::time_point timestamp;
};
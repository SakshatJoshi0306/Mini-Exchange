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

    // Every Trade is fully initialized when created. ->using constructor
    // The timestamp is captured automatically.

    Trade(int buyID,
          int sellID,
          long tradePrice,
          int tradeQuantity)
        : buyOrderID(buyID),
          sellOrderID(sellID),
          price(tradePrice),
          quantity(tradeQuantity),
          timestamp(std::chrono::system_clock::now())
    {
    }
};

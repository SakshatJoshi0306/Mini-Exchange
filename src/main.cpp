#include <iostream>
#include <string>

#include "MatchingEngine.hpp"

int main()
{
    MatchingEngine engine;
    int orderID = 1;

    while (true)
    {
        std::string sideInput;
        std::string typeInput;

        std::cout << "\nBUY / SELL / EXIT : ";
        std::cin >> sideInput;

        if (sideInput == "EXIT")
            break;

        std::cout << "LIMIT / MARKET : ";
        std::cin >> typeInput;

        int quantity;
        std::cout << "Quantity : ";
        std::cin >> quantity;

        // ---------------- Determine Side ----------------

        Side side;

        if (sideInput == "BUY")
            side = Side::BUY;
        else
            side = Side::SELL;

        // ---------------- Determine Order Type ----------------

        OrderType type;

        if (typeInput == "LIMIT")
            type = OrderType::LIMIT;
        else
            type = OrderType::MARKET;

        Order order(
            0,
            Side::BUY,
            0,
            0,
            OrderType::LIMIT
        ); // Dummy initialization. We'll overwrite it below.

        // ---------------- Construct Order ----------------

        if (type == OrderType::LIMIT)
        {
            long price;

            std::cout << "Price : ";
            std::cin >> price;

            order = Order(
                orderID++,
                side,
                quantity,
                price,
                type
            );
        }
        else
        {
            // Price is ignored for market orders.
            order = Order(
                orderID++,
                side,
                quantity,
                0,
                type
            );
        }

        // ---------------- Submit Order ----------------

        engine.ProcessOrder(order);

        // ---------------- Display Book ----------------

        engine.getOrderBook().printBook();
    }

    return 0;
}
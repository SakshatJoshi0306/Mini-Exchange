#include <iostream>
#include <string>

#include "MatchingEngine.hpp"

int main()
{
    MatchingEngine engine;      // One exchange for the lifetime of the program.
    int orderID = 1;            // Simple incremental IDs.

    while (true)
    {
        std::string command;

        std::cout << "\nBUY | SELL | EXIT : ";
        std::cin >> command;

        if(command == "EXIT")
            break;

        int quantity;
        long price;

        std::cout << "Quantity : ";
        std::cin >> quantity;

        std::cout << "Price : ";
        std::cin >> price;

        Side side;

        if(command == "BUY")
            side = Side::BUY;
        else
            side = Side::SELL;

        // Construct the incoming order.
        Order order(
            orderID++,
            side,
            quantity,
            price
        );

        // Let the exchange process it.
        engine.ProcessOrder(order);

        // Print the entire order book after every submission.
        engine.getOrderBook().printBook();
    }

    return 0;
}
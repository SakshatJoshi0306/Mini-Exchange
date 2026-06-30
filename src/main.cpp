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

       std::cout << "\nBUY / SELL / MODIFY / CANCEL / EXIT : ";
        std::cin >> sideInput;

        if (sideInput == "CANCEL")
        {
            int orderID;

            std::cout << "Order ID : ";
            std::cin >> orderID;

            if (engine.cancelOrder(orderID))
                std::cout << "Order cancelled successfully.\n";
            else
                std::cout << "Order not found.\n";

            engine.getOrderBook().printBook();
            continue;
        }
        if (sideInput == "MODIFY")
        {
            int modifyID;
            int newQuantity;
            long newPrice;

            std::cout << "Order ID : ";
            std::cin >> modifyID;

            std::cout << "New Quantity : ";
            std::cin >> newQuantity;

            std::cout << "New Price : ";
            std::cin >> newPrice;

            if (engine.modifyOrder(modifyID, newPrice, newQuantity))
                std::cout << "Order modified successfully.\n";
            else
                std::cout << "Order not found.\n";

            engine.getOrderBook().printBook();

            continue;
        }

        if (sideInput == "EXIT")
            break;

        std::cout << "LIMIT / MARKET / IOC / FOK : ";
        std::cin >> typeInput;

        int quantity;
        std::cout << "Quantity : ";
        std::cin >> quantity;

        // ---------------- Determine Side ----------------

        Side side;

        if (sideInput == "BUY")
        {
            side = Side::BUY;
        }
        else if (sideInput == "SELL")
        {
            side = Side::SELL;
        }
        else
        {
            std::cout << "Invalid command.\n";
            continue;
        }

        // ---------------- Determine Order Type ----------------

        OrderType type;

        if (typeInput == "LIMIT")
    {
        type = OrderType::LIMIT;
    }
    else if (typeInput == "MARKET")
    {
        type = OrderType::MARKET;
    }   
    else if (typeInput == "IOC")
    {
        type = OrderType::IOC;
    }
    else if (typeInput == "FOK")
    {
        type = OrderType::FOK;
    }
        else
        {
            std::cout << "Invalid order type.\n";
            continue;
        }
        Order order(
            0,
            Side::BUY,
            0,
            0,
            OrderType::LIMIT
        ); // Dummy initialization. We'll overwrite it below.

        // ---------------- Construct Order ----------------

        if (type == OrderType::MARKET)
        {
            order = Order(
            orderID++,
            side,
            quantity,
            0,
            type
            );
        }
        else
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

        // ---------------- Submit Order ----------------

        engine.ProcessOrder(order);

        // ---------------- Display Book ----------------

        engine.getOrderBook().printBook();
        engine.printTradeHistory();
        engine.printMarketStatistics();
    }

    return 0;
}
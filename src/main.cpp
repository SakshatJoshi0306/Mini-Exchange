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

        std::cout << "LIMIT / MARKET / IOC / FOK / ICEBERG / STOPMARKET / STOPLIMIT : ";
        std::cin >> typeInput;

        int quantity;
        std::cout << "Quantity : ";
        std::cin >> quantity;

        int clientID;

        std::cout << "Client ID : ";
        std::cin >> clientID;

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
        bool iceberg = false;
        bool stopOrder = false;
        bool stopMarket = false;

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
        else if (typeInput == "ICEBERG")
        {
            type = OrderType::LIMIT;
            iceberg = true;
        }
        else if (typeInput == "STOPMARKET")
        {
        stopOrder = true;
        stopMarket = true;

        // After activation it becomes a MARKET order.
        type = OrderType::MARKET;
        }
        else if(typeInput == "STOPLIMIT")
        {
            stopOrder = true;
            stopMarket = false;

            type = OrderType::LIMIT;
        }
        else
        {       
            std::cout << "Invalid order type.\n";
            continue;
        }
        Order order(
            0,
            0,
            Side::BUY,
            0,
            0,
            OrderType::LIMIT
        ); // Dummy initialization. We'll overwrite it below.

        // ---------------- Construct Order ----------------
        if (stopOrder)
        {
            long triggerPrice;

            std::cout << "Trigger Price : ";
            std::cin >> triggerPrice;

            if (stopMarket)
            {
                order = Order(orderID++, clientID, side, quantity,triggerPrice,true);
            }
            else
            {
                long limitPrice;

                std::cout << "Limit Price : ";
                std::cin >> limitPrice;

                order = Order(orderID++, clientID, side, quantity, triggerPrice, false, limitPrice);
            }
        }
        else if (type == OrderType::MARKET)
        {
            order = Order( orderID++, clientID, side, quantity, 0, type);
        }
        else
        {
            long price;
            std::cout << "Price : ";
            std::cin >> price;

            if (iceberg)
            {
                int peak;

                std::cout << "Peak Size : ";
                std::cin >> peak;

                order = Order(orderID++, clientID, side, quantity, price, type, peak);
            }
            else
            {
                order = Order(orderID++, clientID, side, quantity, price, type);
            }
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
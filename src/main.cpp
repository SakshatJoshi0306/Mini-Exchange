#include <iostream>
#include <string>
#include "Order.hpp"

using namespace std;

int main()
{
    cout << "=====================================\n";
    cout << "    Welcome to Trading Engine v1.0\n";
    cout << "=====================================\n";
    int orderID = 1;

    while (true)
    {
        string command;

        cout << "\nEnter Command (BUY / SELL / EXIT): ";
        cin >> command;

        if (command == "EXIT")
        {
            cout << "Closing Exchange...\n";
            break;
        }

        if (command != "BUY" && command != "SELL")
        {
            cout << "Invalid Command!\n";
            continue;
        }

        int quantity;
        double price;

        cout << "Enter Quantity: ";
        cin >> quantity;

        cout << "Enter Price: ";
        cin >> price;

        Side side;

        if (command == "BUY")
            side = Side::BUY;
        else
            side = Side::SELL;

        Order order(orderID, side, quantity, price);

        cout << "\n----------- ORDER CREATED -----------\n";

        cout << "Order ID : " << order.getId() << endl;

        cout << "Side     : ";

        if (order.getSide() == Side::BUY)
            cout << "BUY\n";
        else
            cout << "SELL\n";

        cout << "Quantity : " << order.getQuantity() << endl;
        cout << "Price    : " << order.getPrice() << endl;

        cout << "-------------------------------------\n";

        orderID++;
    }

    return 0;
}
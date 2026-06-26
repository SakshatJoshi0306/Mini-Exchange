#ifndef ORDER_HPP //This is called the include guard, it tells the compiler If this file has already been included once, ignore it.
#define ORDER_HPP

enum class Side //So that only two legal choices exist for side
{
    BUY,
    SELL
};

class Order //the class contains both private(the information that we dont want to allow other classes to change) and public(information that need to be accessed by other classes) information
{
private:
    int id;
    Side side;
    int quantity;
    double price;

public:
    Order(int id, Side side, int quantity, double price); //Called a Constructor: when i crate an object i dont need to call anything, i can directly use the constructor type Order

    int getId() const;
    Side getSide() const;
    int getQuantity() const;
    double getPrice() const; // all 4 functions are getter functions, simply returns specific information of an Order type object
};

#endif
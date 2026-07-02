#ifndef ORDER_HPP //This is called the include guard, it tells the compiler If this file has already been included once, ignore it.
#define ORDER_HPP

enum class Side //So that only two legal choices exist for side
{
    BUY,
    SELL
};
enum class OrderType //So that order can be distuingished as a limit or a market order
{
    LIMIT,
    MARKET,
    IOC,
    FOK
};
class Order //the class contains both private(the information that we dont want to allow other classes to change) and public(information that need to be accessed by other classes) information
{
private:
    int id;
    Side side;
    // Remaining quantity (visible + hidden).
    int quantity;

    // Quantity currently exposed to the market.
    int visibleQuantity;

    // Maximum visible slice for an iceberg order.
    int peakSize;

    // True if this is an iceberg order.
    bool iceberg;

    // True if this is a stop order.
    bool stopOrder;

    // Trigger price.
    long stopPrice;

    // After triggering, should it become a market order?
     bool stopMarket;

    long priceTicks; //1 price_ is 1 cent, so now we dont have to worry about floating decimals, only dealing with long ints
    
    OrderType orderType;

public:
   Order(int id,
      Side side,
      int quantity,
      long priceTicks,
      OrderType orderType = OrderType::LIMIT); //Called a Constructor: when i crate an object i dont need to call anything, i can directly use the constructor type Order

   Order(int id,
      Side side,
      int quantity,
      long priceTicks,
      OrderType orderType,
      int peakSize);   

    Order(int id,
        Side side,
        int quantity,
        long triggerPrice,
        bool marketAfterTrigger,
        long limitPrice = 0);

    int getId() const;
    Side getSide() const;
    int getQuantity() const;
    long getPriceTicks() const; // all 4 functions are getter functions, simply returns specific information of an Order type object
    void reduceQuantity(int amount);
    bool isFilled() const;
    OrderType getOrderType() const;
    int getVisibleQuantity() const;
    int getRemainingQuantity() const;
    int getPeakSize() const;
    bool isIceberg() const;
    bool needsRefresh() const;
    void refreshVisible();
    bool refreshIfNeeded();
    void initializeVisibleSlice();

    bool isStopOrder() const;
    long getStopPrice() const;
    bool isStopMarket() const;

    void activateStop();
};

#endif
#include "PriceLevel.hpp"

    PriceLevel::PriceLevel(long priceTicks) : priceTicks(priceTicks)
    {
    }

    bool PriceLevel::empty() const
    {
        return orders_.empty();
    }

    long PriceLevel::getPriceTicks() const
    {
        return priceTicks;
    }

    size_t PriceLevel::numberOfOrders() const
    {
        return orders_.size();
    }

    const Order& PriceLevel::frontOrder() const
    {
        return orders_.front();
    }
    
    void PriceLevel::removeFrontOrder() 
    {
        orders_.pop_front();
    }

    void PriceLevel::addOrder(const Order& order)
    {
        orders_.push_back(order);
    }

    int PriceLevel::totalQuantity() const
    {
        int totalquantity{0};
        for(const auto& order : orders_)
        {
            totalquantity += order.getQuantity();
        }
        return totalquantity;
    }   
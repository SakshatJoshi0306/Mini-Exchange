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

    Order& PriceLevel::frontOrder() 
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

    bool PriceLevel::removeOrder(int orderID)
    {
        for (auto it = orders_.begin(); it != orders_.end(); ++it)
        {
            if (it->getId() == orderID)
            {
                orders_.erase(it);
                return true;
            }
        }
        return false;
    }

    std::optional<Order> PriceLevel::findOrder(int orderID) const
    {
        for (const auto& order : orders_)
        {
            if (order.getId() == orderID)
            {
                return order;
            }
        }

        return std::nullopt;
    }

    int PriceLevel::totalVisibleQuantity() const
    {
        int total = 0;

        for (const Order& order : orders_)
        {
            total += order.getVisibleQuantity();
        }

        return total;
    }

    void PriceLevel::processFrontAfterExecution()
{
    if (orders_.empty())
        return;

    Order& front = orders_.front();

    if (front.getVisibleQuantity() > 0)
        return;

    if (front.refreshIfNeeded())
    {
        Order refreshed = std::move(front);

        orders_.pop_front();

        orders_.push_back(std::move(refreshed));
    }
    else
    {
        // No hidden quantity left.
        orders_.pop_front();
    }
}
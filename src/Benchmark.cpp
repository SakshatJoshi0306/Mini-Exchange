#include "Benchmark.hpp"

#include <chrono>
#include <iostream>
#include <random>
#include <iomanip>

Benchmark::Benchmark()
    :
    rng_(std::random_device{}()),
    currentMidPrice_(25000),
    buyProbability_(0.50)
{
}

void Benchmark::moveMarket()
{
    std::uniform_int_distribution<int> move(-1, 1);

    int direction = move(rng_);

    currentMidPrice_ += direction;

    //-----------------------------------------------------
    // Update market sentiment.
    //-----------------------------------------------------

    if (direction == 1)
    {
        buyProbability_ =
            std::min(0.65, buyProbability_ + 0.02);
    }
    else if (direction == -1)
    {
        buyProbability_ =
            std::max(0.35, buyProbability_ - 0.02);
    }
}

Order Benchmark::generateOrder(int orderID)
{
    //==================================================
    // BUY / SELL
    //==================================================

    std::bernoulli_distribution sideDist(buyProbability_);

    Side side =
        sideDist(rng_)
        ?
        Side::BUY
        :
        Side::SELL;

    //==================================================
    // LIMIT / MARKET
    //==================================================
    std::bernoulli_distribution marketDist(0.10);

    OrderType type =
        marketDist(rng_)
        ?
        OrderType::MARKET
        :
        OrderType::LIMIT;

    //==================================================
    // Quantity
    //==================================================

    std::uniform_int_distribution<int> bucket(1,100);

    int quantity;

    int b = bucket(rng_);

    if(b <= 70)
    {
        quantity =
            std::uniform_int_distribution<int>(1,50)(rng_);
    }
    else if(b <= 90)
    {
        quantity =
            std::uniform_int_distribution<int>(50,200)(rng_);
    }
    else
    {
        quantity =
            std::uniform_int_distribution<int>(200,1000)(rng_);
    }

    //==================================================
    // Client
    //==================================================

    int client =
        std::uniform_int_distribution<int>(1,1000)(rng_);

    //==================================================
    // Price
    //==================================================

    if(type == OrderType::MARKET)
    {
        return Order(
            orderID,
            client,
            side,
            quantity,
            0,
            type
        );
    }

    std::normal_distribution<double> offset(0.0, 2.0);

    long price;

    long offsetTicks =
    std::abs(static_cast<long>(std::round(offset(rng_))));

    if (side == Side::BUY)
    {
    // Passive buy orders rest below the mid-price.
        price = currentMidPrice_ - offsetTicks;
    }
    else
    {
    // Passive sell orders rest above the mid-price.
        price = currentMidPrice_ + offsetTicks;
    }

    std::bernoulli_distribution aggressiveDist(0.05);
    std::uniform_int_distribution<int> aggressiveOffset(3, 7);

    if (aggressiveDist(rng_))
    {
        int offset = aggressiveOffset(rng_);

        if (side == Side::BUY)
            price += offset;
        else
            price -= offset;
    }

    return Order(orderID, client, side, quantity, price, type);
}

void Benchmark::run(std::size_t orderCount)
{
    std::cout
    << "\n=========================================\n";

    std::cout
    << "      BENCHMARK CONFIGURATION\n";

    std::cout
    << "=========================================\n\n";

    std::cout
    << "Orders            : "
    << orderCount
    << '\n';

    std::cout
    << "Market Orders     : 10%\n";

    std::cout
    << "Aggressive Limits : 5%\n";

    std::cout
    << "Clients           : 1000\n";

    std::cout
    << "Initial Mid Price : "
    << currentMidPrice_
    << '\n';

    std::cout
    << "=========================================\n\n";


    engine_.setBenchmarkMode(true);

    using clock = std::chrono::high_resolution_clock;

    auto start = clock::now();

    std::bernoulli_distribution driftProbability(0.05);

    for(std::size_t i = 1; i <= orderCount; i++)
    {
        if (driftProbability(rng_))
        {
            moveMarket();
        }

        Order order = generateOrder(i);

         engine_.ProcessOrder(order);
    }

    auto end = clock::now();

    double seconds = std::chrono::duration<double>(end-start).count();

    double averageLatencyMicroseconds = seconds * 1'000'000.0 / orderCount;

    double executionRatio = 100.0 * engine_.tradeCount() / orderCount;

    std::cout
        << "\n=========================\n";

    std::cout
        << "BENCHMARK RESULTS\n";

    std::cout
        << "=========================\n\n";

    std::cout
        << "Orders Processed : "
        << orderCount
        << '\n';

    std::cout
        << "Trade Executions  : "
        << engine_.tradeCount()
        << '\n';

    std::cout
        << "Elapsed Time     : "
        << std::fixed
        << std::setprecision(4)
        << seconds
        << " s\n";

    std::cout
        << "Orders / Second  : "
        << static_cast<long long>(orderCount / seconds)
        << '\n';

    std::cout
    << "Average Latency  : "
    << std::fixed
    << std::setprecision(2)
    << averageLatencyMicroseconds
    << " us/order\n";    

    const OrderBook& book = engine_.getOrderBook();

    std::cout
    << "Resting Orders   : "
    << book.restingOrders()
    << '\n';

    std::cout
    << "Price Levels     : "
    << book.priceLevelCount()
    << '\n';

    std::cout
    << "Avg Queue Length : "
    << book.averageQueueLength()
    << '\n';

    std::cout
    << "Execution Ratio  : "
    << std::fixed
    << std::setprecision(2)
    << executionRatio
    << "%\n";
    
    std::cout << "=========================\n";

}
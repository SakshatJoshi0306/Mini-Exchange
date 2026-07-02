#pragma once

#include <random>

#include "MatchingEngine.hpp"

class Benchmark
{
public:

    Benchmark();

    void run(std::size_t orderCount);

private:

    Order generateOrder(int orderID);

    void moveMarket();

private:

    MatchingEngine engine_;

    std::mt19937 rng_;

    long currentMidPrice_;

    double buyProbability_;
};
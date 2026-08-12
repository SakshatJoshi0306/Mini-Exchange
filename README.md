# Trading Engine

A C++17 project where I built a simplified electronic trading exchange from scratch to learn modern C++, market microstructure and low-latency systems.

## Features

* Price-time priority order matching
* Separate bid/ask order books with FIFO queues
* Market and limit orders
* IOC and FOK orders
* Iceberg orders
* Stop-market and stop-limit orders
* Order cancellation and modification
* Self-match prevention
* Trade history and market statistics
* Market-like synthetic order flow for benchmarking

## How it works

The matching engine maintains separate bid and ask books organized by price level. Orders at the same price are processed in FIFO order.

The main data structures are:

* std::map for ordered price levels
* std::deque for FIFO order queues
* std::unordered_map for fast order lookup

Incoming orders are matched against the best available prices. Partially filled orders remain in the book with their remaining quantity.

## Benchmarking

The project includes a benchmark that generates market-like order flow using different order sizes, prices, client identities, market orders and aggressive limit orders.

It tracks:

* Throughput
* Processing latency
* Trade executions
* Execution ratio
* Resting orders
* Price levels
* Average queue length

This provides a consistent workload for testing correctness and measuring performance changes.

## Build

The project uses CMake and requires a C++17-compatible compiler.

## Project Goals

The main goal of the project is to understand how a real exchange's core matching infrastructure works while getting hands-on experience with:

* Modern C++
* Data structures and algorithms
* Market microstructure
* Systems design
* Performance measurement and optimization

## Future Work

* TCP networking and exchange gateway
* Concurrent client sessions
* Market-data dissemination
* Order-flow replay and backtesting
* Further latency and memory optimizations

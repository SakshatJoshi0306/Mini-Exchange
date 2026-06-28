# Mini Electronic Exchange: 2-Week Deepening Plan

To turn the simple 3-day sprint into an *interview-grade* project, we extend to two weeks, adding performance measurement, real concurrency, and rigorous testing. Below is the **updated schedule** and a detailed **Module 2** walkthrough integrating all lessons learned. Citations are provided for key points.

## Updated Project Timeline (Days 1–14)

- **Days 1–6 (Core Features with Testing):**  Build Modules 1–15 (same finance logic as before) at a measured pace. **Key change:** write unit tests (using Catch2 or Google Test) from Day 1 onward, not “print-and-eye-ball”.  
  - *Day 1:* Project setup (CMake, Catch2). Module 1 (Order class) and Module 2 (OrderBook) implementations, **with tests** (no more “show cout”).  
  - *Day 2:* Module 3 (Matching Engine) and Module 4 (Market Orders).  
  - *Day 3:* Module 5 (Cancel), Module 6 (Modify), Module 7 (Trade History).  
  - *Day 4:* Module 8 (VWAP/Stats), Module 9 (Time & Sales), Module 10 (Level-II Depth).  
  - *Day 5:* Module 11 (IOC/FOK), Module 12 (Iceberg Orders).  
  - *Day 6:* Module 13 (Stop Orders), Module 14 (Logging), Module 15 (CSV Export).  
  - *(Fix tick-based pricing on Day 1:* always use integer `long priceTicks` for prices to avoid floating issues, and stick to it consistently.)*  

- **Days 7–10 (Performance and Profiling):**  Add benchmarking and optimizations.  
  - *Day 7:* Benchmark harness (use `<chrono>` timing) feeding large synthetic order streams. Record *baseline* throughput (orders/sec) and latency (p50/p99).  
  - *Day 8:* Profile hot spots. Likely candidates: map insert/erase (memory alloc), container operations in `PriceLevel` (vector shift). Apply fixes: e.g. use `std::deque` instead of `vector` (constant-time pop front); pre-allocate containers; or use a simple object pool for `Order`. Re-benchmark and note the *improvement*.  
  - *Days 9–10:* **Concurrency layer.** Build a lock-free Single-Producer/Single-Consumer (SPSC) queue between the I/O threads and the matching engine. This lets multiple network threads enqueue orders without locking the book, and a single matching thread dequeue them in order. (An SPSC queue is “easier to reason about” and high-performance.) Benchmark again under concurrent load for a third datapoint.

- **Days 11–13 (Networking & Load Testing):**  
  - *Day 11:* Enhance TCP server to handle **multiple clients** (e.g. one thread per connection). Each client thread parses incoming orders and pushes them into the SPSC queue. The matching engine thread processes from that queue.  
  - *Day 12:* Build a **load generator**: a separate program that spawns N client threads sending random order flows to the server. Use it to stress-test under high rate and varied price distributions.  
  - *Day 13:* Integrate logging (Module 14) and CSV export (Module 15). Run full end-to-end: load generator → TCP server → SPSC queue → matching engine → market data + logs. Fix issues uncovered.

- **Day 14 (Documentation & Interview Prep):**  Write the README and diagrams for an interviewer. Include the architecture (showing the SPSC queue and threading model), benchmark results (before/after), and concise notes on your hardest design decisions (e.g. why deque vs vector, concurrency approach, etc.). Push with a clear commit history reflecting iterative development. Practice explaining your project in 5‐minute and 90‐second versions.

This approach adds *depth* to each module (testing, performance, concurrency), exactly the elements that make your project “engineered” rather than just “functional.”  

## Module 2: Order Book (Detailed Walkthrough)

In Module 2 we implement the **order book** (bids and asks depth). Below is a systematic breakdown of the implementation, updated with our new plan and learnings:

### Design and Data Structures

- **Price ticks (integer) vs floating price:**  We store prices as integer ticks (`long priceTicks`), not `double`. This avoids floating-point rounding and makes equality/comparison exact. All code (including tests) should consistently use `priceTicks`.  
- **Book structure:** We maintain two sorted maps: one for bids, one for asks. Each key is a price tick. The value is a `PriceLevel` object containing the queue of orders at that price. For bids, we sort prices **descending** so highest bid is first; for asks, we sort ascending (default) so lowest ask is first. In C++:

  ```cpp
  // Bid book: highest prices first
  std::map<long, PriceLevel, std::greater<long>> bidBook_;
  // Ask book: lowest prices first (default std::less)
  std::map<long, PriceLevel> askBook_;
  ```

  This works because `std::map` is a sorted associative container: by default keys are sorted ascending by `std::less<Key>`. Using `std::greater<long>` in the template flips it to descending for bids.

- **PriceLevel class:** A helper class holds all orders at one price. It contains a `long priceTicks` and a container of `Order` objects, with FIFO order. We choose `std::deque<Order>` inside `PriceLevel`, because deque supports *constant-time* push/pop at both ends. (A `std::vector` would require shifting elements on pop_front, which is slower for front removals. In general, deque is best when you need efficient pops at both ends.)

- **No `using namespace std` in headers:** As a coding best practice, **never** put `using namespace std;` in a header file. It pollutes the global namespace of every file that includes that header, causing potential name conflicts. Instead, fully qualify (`std::`) in headers or put `using namespace std;` only in `.cpp` files or functions.

### Implementing `addOrder`

The core `OrderBook::addOrder(const Order& o)` inserts a new order into the book:

```cpp
void OrderBook::addOrder(const Order& order) {
    long price = order.getPriceTicks();
    if (order.getSide() == Side::BUY) {
        // Bids (descending map)
        auto it = bidBook_.find(price);
        if (it == bidBook_.end()) {
            // Price level not present – create and insert
            PriceLevel level(price);
            level.addOrder(order);
            bidBook_.insert({price, std::move(level)});
        } else {
            // Existing price level – append order (FIFO)
            it->second.addOrder(order);
        }
    } else {
        // Asks (ascending map)
        auto it = askBook_.find(price);
        if (it == askBook_.end()) {
            PriceLevel level(price);
            level.addOrder(order);
            askBook_.insert({price, std::move(level)});
        } else {
            it->second.addOrder(order);
        }
    }
}
```

**Notes:**
- We look up the price in the appropriate map (`bidBook_` or `askBook_`). If not found, we create a new `PriceLevel` and insert `{price, level}`. Otherwise, `it` points to a `std::pair<const long, PriceLevel>`. We use `it->second` to get the `PriceLevel` object and call `addOrder(order)` on it, preserving time priority.  
- In a map iterator, `it->first` is the price key, and `it->second` is the `PriceLevel` value. For example, `it->second.addOrder(order)` appends the order into the deque for that price level.  
- We ensure all `std::map` insertions preserve order. By using `std::greater<long>` for `bidBook_`, the highest price is `bidBook_.begin()`. For `askBook_`, since it’s default-ascending, the lowest price is `askBook_.begin()`. (Recall: a `std::map` iterator traversal always goes in ascending key order by the comparator.)

### Best-Bid and Best-Ask

We implement functions to query the top of book:

```cpp
long OrderBook::getBestBid() const {
    if (bidBook_.empty()) return -1; // or some null indicator
    return bidBook_.begin()->first;
}

long OrderBook::getBestAsk() const {
    if (askBook_.empty()) return -1;
    return askBook_.begin()->first;
}
```

- Because `bidBook_` is sorted descending (highest first), `bidBook_.begin()->first` is the best bid.  
- Because `askBook_` is sorted ascending (lowest first), `askBook_.begin()->first` is the best ask. We **must not** use `askBook_.end()`: that iterator is one past the last element, not a valid element. (In our development, we accidentally did `askBook_.end()->first` and got garbage; the fix was to use `begin()` instead.)  
- This design mirrors real matching engines, which store bids descending and asks ascending with FIFO queues at each price.

### The `PriceLevel` Class

A simple implementation might look like:

```cpp
class PriceLevel {
public:
    PriceLevel(long price) : price_(price) {}

    void addOrder(const Order& o) {
        orders_.push_back(o);  // FIFO: append to back
        totalQuantity_ += o.getQuantity();
    }

    const Order& frontOrder() const { return orders_.front(); }
    void removeFrontOrder() { 
        totalQuantity_ -= orders_.front().getQuantity();
        orders_.pop_front(); 
    }

    int numberOfOrders() const { return orders_.size(); }
    long totalQuantity() const { return totalQuantity_; }
    bool empty() const { return orders_.empty(); }

private:
    long price_;              // the price level (for reference)
    std::deque<Order> orders_; // orders in time priority
    long totalQuantity_ = 0;
};
```

- We use `std::deque<Order> orders_` so that `pop_front()` and `push_back()` are both **O(1)**. This makes FIFO management efficient.  
- We track `totalQuantity_` for that level (sum of all order quantities), for quick depth queries.

### Testing and Debugging

From Day 1, we write unit tests for Module 2. For example, using Catch2:

```cpp
TEST_CASE("New PriceLevel is empty") {
    PriceLevel lvl(1000);
    REQUIRE(lvl.empty());
    REQUIRE(lvl.numberOfOrders() == 0);
    REQUIRE(lvl.totalQuantity() == 0);
}

TEST_CASE("Adding and popping orders maintains FIFO") {
    PriceLevel lvl(1000);
    lvl.addOrder(Order(1, Side::BUY, 50, 1000));
    lvl.addOrder(Order(2, Side::BUY, 30, 1000));
    REQUIRE(lvl.frontOrder().getId() == 1);
    lvl.removeFrontOrder();
    REQUIRE(lvl.frontOrder().getId() == 2);
}

TEST_CASE("Best bid/ask returns correct prices") {
    OrderBook book;
    book.addOrder(Order(1, Side::BUY, 100, 25000));
    book.addOrder(Order(2, Side::BUY, 50, 25200));
    REQUIRE(book.getBestBid() == 25200);
    book.addOrder(Order(3, Side::SELL, 75, 25100));
    book.addOrder(Order(4, Side::SELL, 10, 24900));
    REQUIRE(book.getBestAsk() == 24900);
}
```

Key lessons from testing:
- Always check corner cases (empty book, single level, etc.).  
- Ensure **bestBid()/bestAsk()** return correct values (beware the `end()` bug we had).  
- We commit code only after tests pass. Example commit message: *“Implement OrderBook with two maps and verify bestBid/ask with unit tests.”* 

Regular testing became critical as we extended the project: it guarantees each module works before moving on, and answers the interview question “how did you verify correctness?” with automated proofs rather than manual prints. This shift to testing-first is one of the major improvements over the 3-day sprint model.

## Key Takeaways for Interviews

When summarizing this project to an interviewer, emphasize these engineered aspects (with data/references where relevant):

- **Automated Testing from Day 1:** We used Catch2 for unit tests on every module (not just printouts). This ensures correctness under arbitrary scenarios (limit, market, cancel, etc.). It shows professionalism and discipline.  
- **Performance Measurement:** We built a simple benchmark to measure orders/sec and latencies. For context, real exchanges handle on the order of ~1 million orders/sec at peak. Our baseline might start lower (e.g. 100k/s in debug mode) but after optimizations (using `std::deque`, preallocation, etc.) we saw substantial improvement. For interviews, you can say “I measured X orders/sec, identified map insertion as a bottleneck, replaced vector with deque and tuned memory, and achieved Y% higher throughput.”  
- **Single-Threaded Matching (Low-Latency Design):** We adopted the industry pattern of **single-threaded matching per symbol** to avoid locking. As noted in exchange design guides, “the matching engine runs single-threaded per symbol — locking is the enemy of latency”. We then decoupled I/O from matching with a lock-free SPSC queue, which is efficient and conceptually simpler. This answers the question “how do you handle multiple clients?”: by serializing orders into one thread, not by locking the book.  
- **Clear Code Practices:** We avoid bad habits like `using namespace std;` in headers (which pollutes all includes). Instead we fully qualify `std::` in interfaces, making the code clean and safe. We also use meaningful data structures (`std::map`, `std::deque`) with the correct comparator (`std::greater<long>` for bids) so that `begin()` always yields the best price.  
- **Scalability and Iteration:** We structured the project in Git with frequent commits after each feature/test. The final README and diagrams document the architecture (order gateway, SPSC queue, matching engine) and the “before/after” performance story. This shows an iterative, data-driven process—exactly the kind of rigour interviewers expect from a portfolio project. 

In summary, beyond the core *what* (an order book and matching engine), the *how* we built it is what counts: test-driven development, careful data-structure choices, measurement and optimization, and a clear concurrency model. Each decision (e.g. map ordering for price-time priority, lock-free queue for concurrency, using deque for O(1) pop_front) was made to optimize for correctness, performance, and scalability. These form a compelling narrative to discuss in interviews.

**Sources:** Authoritative references were used for correctness and best practices, such as the C++ standard library docs (ordering in `std::map`, complexity of `std::deque` operations), exchange design literature (single-threaded matching, target throughput), and community guidance on coding style (avoiding `using namespace std` in headers, SPSC queue rationale). These show that our choices align with industry norms and technical requirements.
cmake_minimum_required(VERSION 3.20)

project(TradingEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------
# Fetch Catch2 automatically
# ---------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

# ---------------------------------------------------
# Include headers
# ---------------------------------------------------

include_directories(include)

# ---------------------------------------------------
# Build library
# ---------------------------------------------------

add_library(TradingEngineLib

    src/Order.cpp
    src/OrderBook.cpp
    src/PriceLevel.cpp
    src/MatchingEngine.cpp
    src/Benchmark.cpp

)

target_include_directories(TradingEngineLib
    PUBLIC
    include
)

# ---------------------------------------------------
# Main executable
# ---------------------------------------------------

add_executable(TradingEngine
    src/main.cpp
)

target_link_libraries(TradingEngine
    PRIVATE
    TradingEngineLib
)

# ---------------------------------------------------
# Enable tests
# ---------------------------------------------------

enable_testing()

add_subdirectory(tests)
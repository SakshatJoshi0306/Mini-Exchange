include(Catch)

file(GLOB TEST_SOURCES "*.cpp")

add_executable(
    TradingEngineTests
    ${TEST_SOURCES}
)

target_link_libraries(
    TradingEngineTests
    PRIVATE
    TradingEngineLib
    Catch2::Catch2WithMain
)

catch_discover_tests(TradingEngineTests)
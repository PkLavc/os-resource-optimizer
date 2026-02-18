cmake_minimum_required(VERSION 3.16)
project(OSResourceOptimizer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags for optimization and warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -O3)
endif()

# Create the main executable
add_executable(os-resource-optimizer
    src/main.cpp
    src/core/process_manager.cpp
    src/core/memory_manager.cpp
    src/core/scheduler.cpp
    src/core/analytics.cpp
    src/core/hardware_simulator.cpp
    src/utils/random_generator.cpp
    src/utils/timer.cpp
)

# Create unit tests
enable_testing()
add_executable(test_runner
    tests/main.cpp
    tests/process_manager_test.cpp
    tests/memory_manager_test.cpp
    tests/scheduler_test.cpp
    tests/analytics_test.cpp
)

# Link test executable with Google Test
find_package(GTest REQUIRED)
target_link_libraries(test_runner GTest::gtest GTest::gtest_main)

# Add test target
add_test(NAME UnitTests COMMAND test_runner)

# Install targets
install(TARGETS os-resource-optimizer
    RUNTIME DESTINATION bin
)

install(TARGETS test_runner
    RUNTIME DESTINATION bin
)
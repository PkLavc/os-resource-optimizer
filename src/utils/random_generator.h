#pragma once

#include <random>
#include <cstdint>

namespace osro {

// Forward declarations
enum class ProcessPriority;
enum class SchedulingAlgorithm;
enum class AllocationStrategy;

namespace osro {

/**
 * @brief Random number generator for simulation parameters
 * 
 * This utility class provides deterministic random number generation
 * for creating realistic simulation scenarios. It ensures reproducible
 * results for benchmarking and testing purposes.
 */
class RandomGenerator {
public:
    /**
     * @brief Construct a new Random Generator
     * @param seed Seed for deterministic generation
     */
    explicit RandomGenerator(uint32_t seed = 42);

    /**
     * @brief Generate random arrival time
     * @param min_time Minimum arrival time
     * @param max_time Maximum arrival time
     * @return uint64_t Random arrival time
     */
    uint64_t generate_arrival_time(uint64_t min_time = 0, uint64_t max_time = 100);

    /**
     * @brief Generate random burst time
     * @param min_time Minimum burst time
     * @param max_time Maximum burst time
     * @return uint64_t Random burst time
     */
    uint64_t generate_burst_time(uint64_t min_time = 1, uint64_t max_time = 1000);

    /**
     * @brief Generate random memory requirement
     * @param min_memory Minimum memory in bytes
     * @param max_memory Maximum memory in bytes
     * @return uint64_t Random memory requirement
     */
    uint64_t generate_memory_requirement(uint64_t min_memory = 1024, uint64_t max_memory = 16777216);

    /**
     * @brief Generate random priority
     * @return ProcessPriority Random priority level
     */
    ProcessPriority generate_priority();

    /**
     * @brief Generate random scheduling algorithm
     * @return SchedulingAlgorithm Random algorithm
     */
    SchedulingAlgorithm generate_algorithm();

    /**
     * @brief Generate random allocation strategy
     * @return AllocationStrategy Random strategy
     */
    AllocationStrategy generate_allocation_strategy();

    /**
     * @brief Set seed for deterministic generation
     * @param seed New seed value
     */
    void set_seed(uint32_t seed);

    /**
     * @brief Get current seed
     * @return uint32_t Current seed
     */
    uint32_t get_seed() const noexcept;

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<uint64_t> arrival_dist_;
    std::uniform_int_distribution<uint64_t> burst_dist_;
    std::uniform_int_distribution<uint64_t> memory_dist_;
    std::uniform_int_distribution<int> priority_dist_;
    std::uniform_int_distribution<int> algorithm_dist_;
    std::uniform_int_distribution<int> strategy_dist_;
};

} // namespace osro
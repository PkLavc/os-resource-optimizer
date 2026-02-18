#pragma once

#include <chrono>
#include <cstdint>

namespace osro {

/**
 * @brief High-precision timer for performance measurement
 * 
 * This utility class provides microsecond-precision timing
 * for accurate performance measurement and benchmarking.
 * It uses the system's high-resolution clock for maximum accuracy.
 */
class Timer {
public:
    /**
     * @brief Construct a new Timer
     */
    Timer();

    /**
     * @brief Start timing
     */
    void start();

    /**
     * @brief Stop timing
     */
    void stop();

    /**
     * @brief Reset timer
     */
    void reset();

    /**
     * @brief Get elapsed time in milliseconds
     * @return uint64_t Elapsed time in milliseconds
     */
    uint64_t get_elapsed_milliseconds() const;

    /**
     * @brief Get elapsed time in microseconds
     * @return uint64_t Elapsed time in microseconds
     */
    uint64_t get_elapsed_microseconds() const;

    /**
     * @brief Get elapsed time in seconds
     * @return double Elapsed time in seconds
     */
    double get_elapsed_seconds() const;

    /**
     * @brief Check if timer is running
     * @return bool True if running
     */
    bool is_running() const noexcept;

private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool running_;
};

} // namespace osro
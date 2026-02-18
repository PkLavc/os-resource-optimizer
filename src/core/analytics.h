#pragma once

#include "process.h"
#include "scheduler.h"
#include "memory_manager.h"
#include <vector>
#include <chrono>
#include <string>

namespace osro {

/**
 * @brief Performance metrics for system analysis
 */
struct PerformanceMetrics {
    double throughput;           // Processes completed per second
    double average_turnaround_time;  // Average time from arrival to completion
    double average_waiting_time;     // Average time spent in ready queue
    double cpu_utilization;      // CPU usage percentage (0.0 to 1.0)
    size_t total_processes;      // Total number of processes
    size_t completed_processes;  // Number of completed processes
    size_t context_switches;     // Total context switches
    double memory_utilization;   // Memory usage percentage (0.0 to 1.0)
    double fragmentation;        // Memory fragmentation percentage (0.0 to 1.0)
    
    PerformanceMetrics()
        : throughput(0.0),
          average_turnaround_time(0.0),
          average_waiting_time(0.0),
          cpu_utilization(0.0),
          total_processes(0),
          completed_processes(0),
          context_switches(0),
          memory_utilization(0.0),
          fragmentation(0.0) {}
};

/**
 * @brief Resource Analytics module for performance measurement
 * 
 * This class calculates key performance indicators for the operating
 * system simulation, including throughput, turnaround time, waiting
 * time, and CPU utilization. It provides the analytical foundation
 * for demonstrating optimization effectiveness.
 */
class ResourceAnalytics {
public:
    /**
     * @brief Construct a new Resource Analytics object
     * @param process_manager Reference to process manager
     * @param scheduler Reference to scheduler
     * @param memory_manager Reference to memory manager
     */
    ResourceAnalytics(ProcessManager& process_manager,
                     Scheduler& scheduler,
                     MemoryManager& memory_manager);

    /**
     * @brief Calculate current performance metrics
     * @return PerformanceMetrics Calculated metrics
     */
    PerformanceMetrics calculate_metrics() const;

    /**
     * @brief Calculate throughput (processes per second)
     * @param time_elapsed Time elapsed in milliseconds
     * @return double Throughput rate
     */
    double calculate_throughput(uint64_t time_elapsed) const;

    /**
     * @brief Calculate average turnaround time
     * @return double Average turnaround time in milliseconds
     */
    double calculate_average_turnaround_time() const;

    /**
     * @brief Calculate average waiting time
     * @return double Average waiting time in milliseconds
     */
    double calculate_average_waiting_time() const;

    /**
     * @brief Calculate CPU utilization percentage
     * @param total_time Total simulation time
     * @param idle_time Total idle time
     * @return double CPU utilization (0.0 to 1.0)
     */
    double calculate_cpu_utilization(uint64_t total_time, uint64_t idle_time) const;

    /**
     * @brief Calculate memory utilization percentage
     * @return double Memory utilization (0.0 to 1.0)
     */
    double calculate_memory_utilization() const;

    /**
     * @brief Calculate memory fragmentation percentage
     * @return double Fragmentation percentage (0.0 to 1.0)
     */
    double calculate_fragmentation() const;

    /**
     * @brief Generate performance report
     * @return std::string Formatted performance report
     */
    std::string generate_report() const;

    /**
     * @brief Reset analytics data
     */
    void reset();

    /**
     * @brief Get simulation start time
     * @return uint64_t Start timestamp
     */
    uint64_t get_start_time() const noexcept;

    /**
     * @brief Get simulation end time
     * @return uint64_t End timestamp
     */
    uint64_t get_end_time() const noexcept;

    /**
     * @brief Set simulation time bounds
     * @param start Start timestamp
     * @param end End timestamp
     */
    void set_time_bounds(uint64_t start, uint64_t end);

private:
    ProcessManager& process_manager_;
    Scheduler& scheduler_;
    MemoryManager& memory_manager_;
    
    uint64_t simulation_start_time_;
    uint64_t simulation_end_time_;
    
    /**
     * @brief Calculate total execution time for all processes
     * @return uint64_t Total execution time
     */
    uint64_t calculate_total_execution_time() const;

    /**
     * @brief Calculate total waiting time for all processes
     * @return uint64_t Total waiting time
     */
    uint64_t calculate_total_waiting_time() const;

    /**
     * @brief Format time duration for display
     * @param milliseconds Time in milliseconds
     * @return std::string Formatted time string
     */
    std::string format_time(uint64_t milliseconds) const;
};

} // namespace osro
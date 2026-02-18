#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace osro {

/**
 * @brief Enumeration of process states in the simulation
 */
enum class ProcessState {
    NEW,           // Process created but not yet ready
    READY,         // Process ready to execute
    RUNNING,       // Process currently executing
    BLOCKED,       // Process waiting for I/O or resources
    TERMINATED     // Process completed execution
};

/**
 * @brief Enumeration of process priorities
 */
enum class ProcessPriority {
    LOW = 1,
    MEDIUM = 5,
    HIGH = 10,
    CRITICAL = 15
};

/**
 * @brief Represents a simulated process in the operating system
 * 
 * This class models a process with its execution characteristics,
 * memory requirements, and scheduling parameters. It demonstrates
 * core OS concepts including process states, priority levels, and
 * execution tracking.
 */
class Process {
public:
    /**
     * @brief Construct a new Process object
     * 
     * @param pid Unique process identifier
     * @param arrival_time Time when process arrives in system
     * @param burst_time Total CPU time required for completion
     * @param memory_required Memory allocation requirement in bytes
     * @param priority Process priority level
     */
    Process(uint32_t pid, 
            uint64_t arrival_time,
            uint64_t burst_time,
            uint64_t memory_required,
            ProcessPriority priority = ProcessPriority::MEDIUM);

    /**
     * @brief Get the process identifier
     * @return uint32_t Process ID
     */
    uint32_t get_pid() const noexcept;

    /**
     * @brief Get the process arrival time
     * @return uint64_t Arrival time in simulation ticks
     */
    uint64_t get_arrival_time() const noexcept;

    /**
     * @brief Get the total burst time required
     * @return uint64_t Total CPU time required
     */
    uint64_t get_burst_time() const noexcept;

    /**
     * @brief Get remaining burst time
     * @return uint64_t Remaining CPU time needed
     */
    uint64_t get_remaining_time() const noexcept;

    /**
     * @brief Get memory requirement
     * @return uint64_t Memory required in bytes
     */
    uint64_t get_memory_required() const noexcept;

    /**
     * @brief Get process priority
     * @return ProcessPriority Priority level
     */
    ProcessPriority get_priority() const noexcept;

    /**
     * @brief Get current process state
     * @return ProcessState Current state
     */
    ProcessState get_state() const noexcept;

    /**
     * @brief Get process name
     * @return const std::string& Process name
     */
    const std::string& get_name() const noexcept;

    /**
     * @brief Set process state
     * @param state New state
     */
    void set_state(ProcessState state) noexcept;

    /**
     * @brief Set process name
     * @param name Process name
     */
    void set_name(const std::string& name);

    /**
     * @brief Execute process for specified time slice
     * @param time_slice Time to execute
     * @return true if process completed, false otherwise
     */
    bool execute(uint64_t time_slice);

    /**
     * @brief Get turnaround time (completion - arrival)
     * @return uint64_t Turnaround time
     */
    uint64_t get_turnaround_time() const noexcept;

    /**
     * @brief Get waiting time (time spent in ready queue)
     * @return uint64_t Waiting time
     */
    uint64_t get_waiting_time() const noexcept;

    /**
     * @brief Get completion time
     * @return uint64_t Completion time
     */
    uint64_t get_completion_time() const noexcept;

    /**
     * @brief Set completion time
     * @param time Completion time
     */
    void set_completion_time(uint64_t time) noexcept;

    /**
     * @brief Check if process is completed
     * @return true if completed, false otherwise
     */
    bool is_completed() const noexcept;

    /**
     * @brief Get execution history
     * @return const std::vector<uint64_t>& Execution timestamps
     */
    const std::vector<uint64_t>& get_execution_history() const noexcept;

    /**
     * @brief Add execution timestamp to history
     * @param timestamp Execution timestamp
     */
    void add_execution_timestamp(uint64_t timestamp);

private:
    uint32_t pid_;
    uint64_t arrival_time_;
    uint64_t burst_time_;
    uint64_t remaining_time_;
    uint64_t memory_required_;
    ProcessPriority priority_;
    ProcessState state_;
    std::string name_;
    uint64_t completion_time_;
    std::vector<uint64_t> execution_history_;
};

/**
 * @brief Comparison operators for process scheduling algorithms
 */
bool operator<(const Process& lhs, const Process& rhs);
bool operator>(const Process& lhs, const Process& rhs);
bool operator==(const Process& lhs, const Process& rhs);

} // namespace osro
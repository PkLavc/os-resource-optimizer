#pragma once

#include "process.h"
#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <functional>

namespace osro {

/**
 * @brief Manages the lifecycle and state transitions of processes
 * 
 * This class implements the process control block (PCB) management
 * functionality, handling process creation, state transitions,
 * and process termination. It serves as the central authority
 * for process management in the simulation.
 */
class ProcessManager {
public:
    /**
     * @brief Construct a new Process Manager
     */
    ProcessManager();

    /**
     * @brief Create a new process
     * @param arrival_time Time when process arrives
     * @param burst_time Total CPU time required
     * @param memory_required Memory allocation requirement
     * @param priority Process priority level
     * @return Process* Pointer to the created process
     */
    Process* create_process(uint64_t arrival_time,
                           uint64_t burst_time,
                           uint64_t memory_required,
                           ProcessPriority priority = ProcessPriority::MEDIUM);

    /**
     * @brief Get process by ID
     * @param pid Process identifier
     * @return Process* Pointer to process or nullptr if not found
     */
    Process* get_process(uint32_t pid) const;

    /**
     * @brief Remove and destroy process
     * @param pid Process identifier
     * @return true if process was found and removed
     */
    bool destroy_process(uint32_t pid);

    /**
     * @brief Get all processes
     * @return const std::vector<Process*>& Vector of all processes
     */
    const std::vector<Process*>& get_all_processes() const;

    /**
     * @brief Get processes by state
     * @param state Target state
     * @return std::vector<Process*> Processes in the specified state
     */
    std::vector<Process*> get_processes_by_state(ProcessState state) const;

    /**
     * @brief Get number of processes
     * @return size_t Total number of processes
     */
    size_t get_process_count() const noexcept;

    /**
     * @brief Get number of completed processes
     * @return size_t Number of terminated processes
     */
    size_t get_completed_count() const noexcept;

    /**
     * @brief Clean up all terminated processes
     * @return size_t Number of processes cleaned up
     */
    size_t cleanup_terminated();

    /**
     * @brief Reset the process manager
     */
    void reset();

private:
    std::vector<std::unique_ptr<Process>> processes_;
    std::unordered_map<uint32_t, Process*> process_map_;
    uint32_t next_pid_;
};

} // namespace osro
#pragma once

#include "process.h"
#include "process_manager.h"
#include <queue>
#include <vector>
#include <functional>
#include <memory>

namespace osro {

/**
 * @brief Enumeration of scheduling algorithms
 */
enum class SchedulingAlgorithm {
    ROUND_ROBIN,      // Time-slice based scheduling
    PRIORITY,         // Priority-based scheduling
    SHORTEST_JOB_FIRST // SJF scheduling
};

/**
 * @brief Represents a scheduling event
 */
struct ScheduleEvent {
    uint64_t timestamp;
    Process* process;
    ProcessState old_state;
    ProcessState new_state;
    
    ScheduleEvent(uint64_t time, Process* proc, ProcessState old_s, ProcessState new_s)
        : timestamp(time), process(proc), old_state(old_s), new_state(new_s) {}
};

/**
 * @brief Implements CPU scheduling algorithms
 * 
 * This class provides multiple scheduling algorithms including
 * Round Robin, Priority-based, and Shortest Job First. It simulates
 * context switching and maintains scheduling history for analysis.
 */
class Scheduler {
public:
    /**
     * @brief Construct a new Scheduler
     * @param algorithm Scheduling algorithm to use
     * @param time_slice Time slice for Round Robin (default 10ms)
     */
    Scheduler(SchedulingAlgorithm algorithm, uint64_t time_slice = 10);

    /**
     * @brief Set scheduling algorithm
     * @param algorithm New algorithm
     */
    void set_algorithm(SchedulingAlgorithm algorithm);

    /**
     * @brief Set time slice for Round Robin
     * @param time_slice Time slice in milliseconds
     */
    void set_time_slice(uint64_t time_slice);

    /**
     * @brief Add process to ready queue
     * @param process Process to add
     */
    void add_to_ready_queue(Process* process);

    /**
     * @brief Get next process to execute
     * @return Process* Process to execute, nullptr if queue empty
     */
    Process* get_next_process();

    /**
     * @brief Remove process from ready queue
     * @param process Process to remove
     * @return bool True if process was in queue
     */
    bool remove_from_ready_queue(Process* process);

    /**
     * @brief Check if ready queue is empty
     * @return bool True if empty
     */
    bool is_ready_queue_empty() const;

    /**
     * @brief Get ready queue size
     * @return size_t Number of processes in queue
     */
    size_t get_ready_queue_size() const;

    /**
     * @brief Clear ready queue
     */
    void clear_ready_queue();

    /**
     * @brief Get scheduling history
     * @return const std::vector<ScheduleEvent>& Scheduling events
     */
    const std::vector<ScheduleEvent>& get_schedule_history() const;

    /**
     * @brief Perform context switch simulation
     * @param from Process switching from (can be nullptr)
     * @param to Process switching to (can be nullptr)
     * @param timestamp Current timestamp
     * @return uint64_t Context switch overhead in milliseconds
     */
    uint64_t simulate_context_switch(Process* from, Process* to, uint64_t timestamp);

    /**
     * @brief Get total context switches
     * @return size_t Number of context switches
     */
    size_t get_context_switch_count() const noexcept;

    /**
     * @brief Reset scheduler state
     */
    void reset();

private:
    SchedulingAlgorithm algorithm_;
    uint64_t time_slice_;
    size_t context_switches_;
    
    std::queue<Process*> ready_queue_;
    std::vector<ScheduleEvent> schedule_history_;
    
    /**
     * @brief Compare processes for priority scheduling
     */
    struct PriorityComparator {
        bool operator()(const Process* a, const Process* b) const {
            return *a < *b; // Higher priority first
        }
    };
    
    /**
     * @brief Compare processes for SJF scheduling
     */
    struct SJFComparator {
        bool operator()(const Process* a, const Process* b) const {
            return a->get_remaining_time() > b->get_remaining_time(); // Shorter job first
        }
    };
    
    /**
     * @brief Get priority queue for current algorithm
     * @return std::priority_queue<Process*, std::vector<Process*>, Comparator>&
     */
    template<typename Comparator>
    std::priority_queue<Process*, std::vector<Process*>, Comparator>& 
    get_priority_queue();
    
    /**
     * @brief Convert queue to priority queue for algorithm change
     */
    void convert_to_priority_queue();
    
    /**
     * @brief Record scheduling event
     * @param process Process involved
     * @param old_state Previous state
     * @param new_state New state
     * @param timestamp Event timestamp
     */
    void record_event(Process* process, ProcessState old_state, 
                     ProcessState new_state, uint64_t timestamp);
};

} // namespace osro
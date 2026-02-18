#pragma once

#include "process.h"
#include "scheduler.h"
#include "memory_manager.h"
#include <functional>
#include <vector>
#include <queue>

namespace osro {

/**
 * @brief Enumeration of interrupt types
 */
enum class InterruptType {
    TIMER,        // Timer interrupt for time-slicing
    I_O,          // I/O completion interrupt
    SYSTEM_CALL,  // System call interrupt
    HARDWARE_FAULT // Hardware error interrupt
};

/**
 * @brief Represents an interrupt event
 */
struct Interrupt {
    uint64_t timestamp;
    InterruptType type;
    uint32_t source_id;  // Process ID or device ID
    std::string description;
    
    Interrupt(uint64_t time, InterruptType itype, uint32_t source, const std::string& desc)
        : timestamp(time), type(itype), source_id(source), description(desc) {}
};

/**
 * @brief Hardware Simulator for interrupt and context switching simulation
 * 
 * This class simulates hardware-level operations including interrupt
 * handling, context switching, and device management. It provides
 * the low-level foundation for demonstrating real-time system behavior
 * and interrupt-driven processing.
 */
class HardwareSimulator {
public:
    /**
     * @brief Construct a new Hardware Simulator
     * @param scheduler Reference to scheduler
     * @param memory_manager Reference to memory manager
     */
    HardwareSimulator(Scheduler& scheduler, MemoryManager& memory_manager);

    /**
     * @brief Simulate timer interrupt for time-slicing
     * @param current_process Currently running process
     * @param timestamp Current timestamp
     * @return uint64_t Context switch overhead
     */
    uint64_t simulate_timer_interrupt(Process* current_process, uint64_t timestamp);

    /**
     * @brief Simulate I/O interrupt
     * @param process_id Process ID that initiated I/O
     * @param timestamp Timestamp of interrupt
     * @return bool True if interrupt handled successfully
     */
    bool simulate_io_interrupt(uint32_t process_id, uint64_t timestamp);

    /**
     * @brief Simulate system call
     * @param process_id Process making the system call
     * @param call_type Type of system call
     * @param timestamp Timestamp of call
     * @return uint64_t System call overhead
     */
    uint64_t simulate_system_call(uint32_t process_id, const std::string& call_type, uint64_t timestamp);

    /**
     * @brief Simulate hardware fault
     * @param fault_description Description of the fault
     * @param timestamp Timestamp of fault
     * @return bool True if fault handled
     */
    bool simulate_hardware_fault(const std::string& fault_description, uint64_t timestamp);

    /**
     * @brief Process pending interrupts
     * @param current_time Current simulation time
     * @return size_t Number of interrupts processed
     */
    size_t process_interrupts(uint64_t current_time);

    /**
     * @brief Schedule interrupt
     * @param interrupt Interrupt to schedule
     */
    void schedule_interrupt(const Interrupt& interrupt);

    /**
     * @brief Get interrupt queue size
     * @return size_t Number of pending interrupts
     */
    size_t get_pending_interrupts() const;

    /**
     * @brief Clear all pending interrupts
     */
    void clear_interrupts();

    /**
     * @brief Get interrupt statistics
     * @return std::vector<Interrupt> Processed interrupts
     */
    const std::vector<Interrupt>& get_interrupt_history() const;

    /**
     * @brief Simulate context switch at hardware level
     * @param from Process switching from
     * @param to Process switching to
     * @param timestamp Timestamp of switch
     * @return uint64_t Hardware context switch time
     */
    uint64_t simulate_hardware_context_switch(Process* from, Process* to, uint64_t timestamp);

    /**
     * @brief Get total hardware overhead
     * @return uint64_t Total overhead in milliseconds
     */
    uint64_t get_total_overhead() const noexcept;

    /**
     * @brief Reset hardware simulator
     */
    void reset();

private:
    Scheduler& scheduler_;
    MemoryManager& memory_manager_;
    
    std::priority_queue<Interrupt, std::vector<Interrupt>, 
                       std::function<bool(const Interrupt&, const Interrupt&)>>
        interrupt_queue_;
    
    std::vector<Interrupt> interrupt_history_;
    uint64_t total_overhead_;
    
    /**
     * @brief Comparator for interrupt priority queue
     */
    static bool interrupt_comparator(const Interrupt& a, const Interrupt& b) {
        return a.timestamp > b.timestamp; // Min-heap based on timestamp
    }
    
    /**
     * @brief Handle timer interrupt
     * @param interrupt Timer interrupt to handle
     * @return uint64_t Overhead time
     */
    uint64_t handle_timer_interrupt(const Interrupt& interrupt);
    
    /**
     * @brief Handle I/O interrupt
     * @param interrupt I/O interrupt to handle
     * @return uint64_t Overhead time
     */
    uint64_t handle_io_interrupt(const Interrupt& interrupt);
    
    /**
     * @brief Handle system call interrupt
     * @param interrupt System call interrupt to handle
     * @return uint64_t Overhead time
     */
    uint64_t handle_system_call_interrupt(const Interrupt& interrupt);
    
    /**
     * @brief Handle hardware fault interrupt
     * @param interrupt Fault interrupt to handle
     * @return uint64_t Overhead time
     */
    uint64_t handle_hardware_fault_interrupt(const Interrupt& interrupt);
    
    /**
     * @brief Simulate memory management unit operation
     * @param process_id Process ID
     * @param virtual_address Virtual address
     * @return uint64_t Physical address
     */
    uint64_t simulate_mmu_translation(uint32_t process_id, uint64_t virtual_address);
};

} // namespace osro
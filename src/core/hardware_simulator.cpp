#include "hardware_simulator.h"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>

namespace osro {

HardwareSimulator::HardwareSimulator(Scheduler& scheduler, MemoryManager& memory_manager)
    : scheduler_(scheduler),
      memory_manager_(memory_manager),
      interrupt_queue_(interrupt_comparator),
      total_overhead_(0) {}

uint64_t HardwareSimulator::simulate_timer_interrupt(Process* current_process, uint64_t timestamp) {
    Interrupt timer_interrupt(timestamp, InterruptType::TIMER, 
                             (current_process ? current_process->get_pid() : 0),
                             "Timer slice expired");
    
    schedule_interrupt(timer_interrupt);
    return handle_timer_interrupt(timer_interrupt);
}

bool HardwareSimulator::simulate_io_interrupt(uint32_t process_id, uint64_t timestamp) {
    Interrupt io_interrupt(timestamp, InterruptType::I_O, process_id, "I/O operation completed");
    schedule_interrupt(io_interrupt);
    return (handle_io_interrupt(io_interrupt) > 0);
}

uint64_t HardwareSimulator::simulate_system_call(uint32_t process_id, 
                                               const std::string& call_type, 
                                               uint64_t timestamp) {
    Interrupt syscall_interrupt(timestamp, InterruptType::SYSTEM_CALL, process_id, 
                              "System call: " + call_type);
    schedule_interrupt(syscall_interrupt);
    return handle_system_call_interrupt(syscall_interrupt);
}

bool HardwareSimulator::simulate_hardware_fault(const std::string& fault_description, uint64_t timestamp) {
    Interrupt fault_interrupt(timestamp, InterruptType::HARDWARE_FAULT, 0, fault_description);
    schedule_interrupt(fault_interrupt);
    return (handle_hardware_fault_interrupt(fault_interrupt) > 0);
}

size_t HardwareSimulator::process_interrupts(uint64_t current_time) {
    size_t processed = 0;
    
    while (!interrupt_queue_.empty() && interrupt_queue_.top().timestamp <= current_time) {
        Interrupt interrupt = interrupt_queue_.top();
        interrupt_queue_.pop();
        
        uint64_t overhead = 0;
        switch (interrupt.type) {
            case InterruptType::TIMER:
                overhead = handle_timer_interrupt(interrupt);
                break;
            case InterruptType::I_O:
                overhead = handle_io_interrupt(interrupt);
                break;
            case InterruptType::SYSTEM_CALL:
                overhead = handle_system_call_interrupt(interrupt);
                break;
            case InterruptType::HARDWARE_FAULT:
                overhead = handle_hardware_fault_interrupt(interrupt);
                break;
        }
        
        total_overhead_ += overhead;
        interrupt_history_.push_back(interrupt);
        processed++;
    }
    
    return processed;
}

void HardwareSimulator::schedule_interrupt(const Interrupt& interrupt) {
    interrupt_queue_.push(interrupt);
}

size_t HardwareSimulator::get_pending_interrupts() const {
    return interrupt_queue_.size();
}

void HardwareSimulator::clear_interrupts() {
    while (!interrupt_queue_.empty()) {
        interrupt_queue_.pop();
    }
    interrupt_history_.clear();
}

const std::vector<Interrupt>& HardwareSimulator::get_interrupt_history() const {
    return interrupt_history_;
}

uint64_t HardwareSimulator::simulate_hardware_context_switch(Process* from, Process* to, uint64_t timestamp) {
    // Simulate hardware-level context switch overhead
    uint64_t overhead = 2; // 2ms hardware context switch overhead
    
    // Simulate MMU operations
    if (from) {
        simulate_mmu_translation(from->get_pid(), 0); // Flush TLB
    }
    
    if (to) {
        simulate_mmu_translation(to->get_pid(), 0); // Load page tables
    }
    
    total_overhead_ += overhead;
    return overhead;
}

uint64_t HardwareSimulator::get_total_overhead() const noexcept {
    return total_overhead_;
}

void HardwareSimulator::reset() {
    clear_interrupts();
    total_overhead_ = 0;
}

uint64_t HardwareSimulator::handle_timer_interrupt(const Interrupt& interrupt) {
    // Timer interrupt handling overhead
    return 1; // 1ms overhead
}

uint64_t HardwareSimulator::handle_io_interrupt(const Interrupt& interrupt) {
    // I/O interrupt handling overhead
    return 3; // 3ms overhead
}

uint64_t HardwareSimulator::handle_system_call_interrupt(const Interrupt& interrupt) {
    // System call handling overhead
    return 5; // 5ms overhead
}

uint64_t HardwareSimulator::handle_hardware_fault_interrupt(const Interrupt& interrupt) {
    // Hardware fault handling overhead
    return 10; // 10ms overhead
}

uint64_t HardwareSimulator::simulate_mmu_translation(uint32_t process_id, uint64_t virtual_address) {
    // Simulate MMU address translation
    // In a real system, this would involve page table lookups, TLB operations, etc.
    
    // For simulation, we'll just return a simple transformation
    // Real MMU would involve complex page table walking and caching
    return virtual_address + (process_id * 0x1000);
}

} // namespace osro
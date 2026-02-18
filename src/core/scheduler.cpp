#include "scheduler.h"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>

namespace osro {

Scheduler::Scheduler(SchedulingAlgorithm algorithm, uint64_t time_slice)
    : algorithm_(algorithm),
      time_slice_(time_slice),
      context_switches_(0) {
    
    if (time_slice == 0) {
        throw std::invalid_argument("Time slice must be greater than 0");
    }
}

void Scheduler::set_algorithm(SchedulingAlgorithm algorithm) {
    if (algorithm_ != algorithm) {
        algorithm_ = algorithm;
        convert_to_priority_queue();
    }
}

void Scheduler::set_time_slice(uint64_t time_slice) {
    if (time_slice == 0) {
        throw std::invalid_argument("Time slice must be greater than 0");
    }
    time_slice_ = time_slice;
}

void Scheduler::add_to_ready_queue(Process* process) {
    if (!process) {
        return;
    }
    
    process->set_state(ProcessState::READY);
    record_event(process, ProcessState::NEW, ProcessState::READY, 0);
    
    ready_queue_.push(process);
}

Process* Scheduler::get_next_process() {
    if (ready_queue_.empty()) {
        return nullptr;
    }
    
    Process* process = ready_queue_.front();
    ready_queue_.pop();
    
    process->set_state(ProcessState::RUNNING);
    record_event(process, ProcessState::READY, ProcessState::RUNNING, 0);
    
    return process;
}

bool Scheduler::remove_from_ready_queue(Process* process) {
    if (!process) {
        return false;
    }
    
    // For Round Robin, we need to search the queue
    std::queue<Process*> temp_queue;
    bool found = false;
    
    while (!ready_queue_.empty()) {
        Process* current = ready_queue_.front();
        ready_queue_.pop();
        
        if (current == process) {
            found = true;
            process->set_state(ProcessState::TERMINATED);
            record_event(process, ProcessState::READY, ProcessState::TERMINATED, 0);
        } else {
            temp_queue.push(current);
        }
    }
    
    // Restore remaining processes
    ready_queue_ = std::move(temp_queue);
    
    return found;
}

bool Scheduler::is_ready_queue_empty() const {
    return ready_queue_.empty();
}

size_t Scheduler::get_ready_queue_size() const {
    return ready_queue_.size();
}

void Scheduler::clear_ready_queue() {
    while (!ready_queue_.empty()) {
        Process* process = ready_queue_.front();
        ready_queue_.pop();
        process->set_state(ProcessState::TERMINATED);
        record_event(process, ProcessState::READY, ProcessState::TERMINATED, 0);
    }
}

const std::vector<ScheduleEvent>& Scheduler::get_schedule_history() const {
    return schedule_history_;
}

uint64_t Scheduler::simulate_context_switch(Process* from, Process* to, uint64_t timestamp) {
    // Simulate context switch overhead
    uint64_t overhead = 1; // 1ms context switch overhead
    
    if (from) {
        from->set_state(ProcessState::READY);
        record_event(from, ProcessState::RUNNING, ProcessState::READY, timestamp);
    }
    
    if (to) {
        to->set_state(ProcessState::RUNNING);
        record_event(to, ProcessState::READY, ProcessState::RUNNING, timestamp + overhead);
    }
    
    context_switches_++;
    return overhead;
}

size_t Scheduler::get_context_switch_count() const noexcept {
    return context_switches_;
}

void Scheduler::reset() {
    clear_ready_queue();
    schedule_history_.clear();
    context_switches_ = 0;
}

void Scheduler::convert_to_priority_queue() {
    // This is a simplified implementation
    // In a full implementation, we would convert between queue types
    // based on the algorithm change
}

void Scheduler::record_event(Process* process, ProcessState old_state, 
                           ProcessState new_state, uint64_t timestamp) {
    schedule_history_.emplace_back(timestamp, process, old_state, new_state);
}

} // namespace osro
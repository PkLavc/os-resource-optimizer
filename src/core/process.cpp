#include "process.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace osro {

Process::Process(uint32_t pid, 
                 uint64_t arrival_time,
                 uint64_t burst_time,
                 uint64_t memory_required,
                 ProcessPriority priority)
    : pid_(pid),
      arrival_time_(arrival_time),
      burst_time_(burst_time),
      remaining_time_(burst_time),
      memory_required_(memory_required),
      priority_(priority),
      state_(ProcessState::NEW),
      name_("Process_" + std::to_string(pid)),
      completion_time_(0) {
    
    if (burst_time == 0) {
        throw std::invalid_argument("Burst time must be greater than 0");
    }
    
    if (memory_required == 0) {
        throw std::invalid_argument("Memory requirement must be greater than 0");
    }
}

uint32_t Process::get_pid() const noexcept {
    return pid_;
}

uint64_t Process::get_arrival_time() const noexcept {
    return arrival_time_;
}

uint64_t Process::get_burst_time() const noexcept {
    return burst_time_;
}

uint64_t Process::get_remaining_time() const noexcept {
    return remaining_time_;
}

uint64_t Process::get_memory_required() const noexcept {
    return memory_required_;
}

ProcessPriority Process::get_priority() const noexcept {
    return priority_;
}

ProcessState Process::get_state() const noexcept {
    return state_;
}

const std::string& Process::get_name() const noexcept {
    return name_;
}

void Process::set_state(ProcessState state) noexcept {
    state_ = state;
}

void Process::set_name(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Process name cannot be empty");
    }
    name_ = name;
}

bool Process::execute(uint64_t time_slice) {
    if (state_ != ProcessState::RUNNING) {
        throw std::runtime_error("Process must be in RUNNING state to execute");
    }
    
    if (remaining_time_ <= time_slice) {
        // Process will complete
        remaining_time_ = 0;
        state_ = ProcessState::TERMINATED;
        return true;
    } else {
        // Process continues
        remaining_time_ -= time_slice;
        return false;
    }
}

uint64_t Process::get_turnaround_time() const noexcept {
    if (completion_time_ == 0) {
        return 0; // Not completed yet
    }
    return completion_time_ - arrival_time_;
}

uint64_t Process::get_waiting_time() const noexcept {
    if (completion_time_ == 0) {
        return 0; // Not completed yet
    }
    return get_turnaround_time() - burst_time_;
}

uint64_t Process::get_completion_time() const noexcept {
    return completion_time_;
}

void Process::set_completion_time(uint64_t time) noexcept {
    completion_time_ = time;
}

bool Process::is_completed() const noexcept {
    return state_ == ProcessState::TERMINATED;
}

const std::vector<uint64_t>& Process::get_execution_history() const noexcept {
    return execution_history_;
}

void Process::add_execution_timestamp(uint64_t timestamp) {
    execution_history_.push_back(timestamp);
}

// Comparison operators for scheduling algorithms
bool operator<(const Process& lhs, const Process& rhs) {
    // For priority scheduling: higher priority first
    if (lhs.get_priority() != rhs.get_priority()) {
        return static_cast<int>(lhs.get_priority()) > static_cast<int>(rhs.get_priority());
    }
    
    // For SJF: shorter burst time first
    if (lhs.get_burst_time() != rhs.get_burst_time()) {
        return lhs.get_burst_time() < rhs.get_burst_time();
    }
    
    // For FCFS: earlier arrival first
    return lhs.get_arrival_time() < rhs.get_arrival_time();
}

bool operator>(const Process& lhs, const Process& rhs) {
    return rhs < lhs;
}

bool operator==(const Process& lhs, const Process& rhs) {
    return lhs.get_pid() == rhs.get_pid();
}

} // namespace osro
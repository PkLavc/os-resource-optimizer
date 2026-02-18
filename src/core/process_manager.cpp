#include "process_manager.h"
#include <algorithm>
#include <vector>

namespace osro {

ProcessManager::ProcessManager() : next_pid_(1) {}

Process* ProcessManager::create_process(uint64_t arrival_time,
                                       uint64_t burst_time,
                                       uint64_t memory_required,
                                       ProcessPriority priority) {
    try {
        auto process = std::make_unique<Process>(next_pid_++, 
                                                arrival_time, 
                                                burst_time, 
                                                memory_required, 
                                                priority);
        Process* process_ptr = process.get();
        
        processes_.push_back(std::move(process));
        process_map_[process_ptr->get_pid()] = process_ptr;
        
        return process_ptr;
    } catch (const std::exception& e) {
        // Log error and return nullptr
        return nullptr;
    }
}

Process* ProcessManager::get_process(uint32_t pid) const {
    auto it = process_map_.find(pid);
    return (it != process_map_.end()) ? it->second : nullptr;
}

bool ProcessManager::destroy_process(uint32_t pid) {
    auto it = process_map_.find(pid);
    if (it == process_map_.end()) {
        return false;
    }
    
    Process* process = it->second;
    
    // Remove from map
    process_map_.erase(it);
    
    // Remove from vector (inefficient but simple for simulation)
    auto vec_it = std::find_if(processes_.begin(), processes_.end(),
                              [process](const std::unique_ptr<Process>& p) {
                                  return p.get() == process;
                              });
    
    if (vec_it != processes_.end()) {
        processes_.erase(vec_it);
        return true;
    }
    
    return false;
}

const std::vector<Process*>& ProcessManager::get_all_processes() const {
    static std::vector<Process*> process_ptrs;
    process_ptrs.clear();
    
    for (const auto& process : processes_) {
        process_ptrs.push_back(process.get());
    }
    
    return process_ptrs;
}

std::vector<Process*> ProcessManager::get_processes_by_state(ProcessState state) const {
    std::vector<Process*> result;
    
    for (const auto& process : processes_) {
        if (process->get_state() == state) {
            result.push_back(process.get());
        }
    }
    
    return result;
}

size_t ProcessManager::get_process_count() const noexcept {
    return processes_.size();
}

size_t ProcessManager::get_completed_count() const noexcept {
    return get_processes_by_state(ProcessState::TERMINATED).size();
}

size_t ProcessManager::cleanup_terminated() {
    size_t cleaned = 0;
    auto it = processes_.begin();
    
    while (it != processes_.end()) {
        if ((*it)->get_state() == ProcessState::TERMINATED) {
            uint32_t pid = (*it)->get_pid();
            process_map_.erase(pid);
            it = processes_.erase(it);
            cleaned++;
        } else {
            ++it;
        }
    }
    
    return cleaned;
}

void ProcessManager::reset() {
    processes_.clear();
    process_map_.clear();
    next_pid_ = 1;
}

} // namespace osro
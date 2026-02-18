#include "analytics.h"
#include <numeric>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstddef>

namespace osro {

ResourceAnalytics::ResourceAnalytics(ProcessManager& process_manager,
                                   Scheduler& scheduler,
                                   MemoryManager& memory_manager)
    : process_manager_(process_manager),
      scheduler_(scheduler),
      memory_manager_(memory_manager),
      simulation_start_time_(0),
      simulation_end_time_(0) {}

PerformanceMetrics ResourceAnalytics::calculate_metrics() const {
    PerformanceMetrics metrics;
    
    const auto& all_processes = process_manager_.get_all_processes();
    metrics.total_processes = all_processes.size();
    metrics.completed_processes = process_manager_.get_completed_count();
    metrics.context_switches = scheduler_.get_context_switch_count();
    
    // Calculate throughput
    uint64_t time_elapsed = simulation_end_time_ - simulation_start_time_;
    metrics.throughput = calculate_throughput(time_elapsed);
    
    // Calculate average times
    metrics.average_turnaround_time = calculate_average_turnaround_time();
    metrics.average_waiting_time = calculate_average_waiting_time();
    
    // Calculate utilization
    metrics.cpu_utilization = calculate_cpu_utilization(time_elapsed, 0); // Simplified
    metrics.memory_utilization = calculate_memory_utilization();
    metrics.fragmentation = calculate_fragmentation();
    
    return metrics;
}

double ResourceAnalytics::calculate_throughput(uint64_t time_elapsed) const {
    if (time_elapsed == 0) return 0.0;
    
    size_t completed = process_manager_.get_completed_count();
    // Convert milliseconds to seconds and calculate processes per second
    return static_cast<double>(completed) / (time_elapsed / 1000.0);
}

double ResourceAnalytics::calculate_average_turnaround_time() const {
    const auto& all_processes = process_manager_.get_all_processes();
    if (all_processes.empty()) return 0.0;
    
    double total_time = 0.0;
    size_t completed_count = 0;
    
    for (const auto* process : all_processes) {
        if (process->is_completed()) {
            total_time += process->get_turnaround_time();
            completed_count++;
        }
    }
    
    return (completed_count > 0) ? total_time / completed_count : 0.0;
}

double ResourceAnalytics::calculate_average_waiting_time() const {
    const auto& all_processes = process_manager_.get_all_processes();
    if (all_processes.empty()) return 0.0;
    
    double total_time = 0.0;
    size_t completed_count = 0;
    
    for (const auto* process : all_processes) {
        if (process->is_completed()) {
            total_time += process->get_waiting_time();
            completed_count++;
        }
    }
    
    return (completed_count > 0) ? total_time / completed_count : 0.0;
}

double ResourceAnalytics::calculate_cpu_utilization(uint64_t total_time, uint64_t idle_time) const {
    if (total_time == 0) return 0.0;
    
    double busy_time = static_cast<double>(total_time - idle_time);
    return busy_time / total_time;
}

double ResourceAnalytics::calculate_memory_utilization() const {
    return memory_manager_.get_utilization();
}

double ResourceAnalytics::calculate_fragmentation() const {
    return memory_manager_.get_fragmentation();
}

std::string ResourceAnalytics::generate_report() const {
    auto metrics = calculate_metrics();
    std::ostringstream report;
    
    report << std::fixed << std::setprecision(2);
    report << "=== OS Resource Optimizer Performance Report ===\n";
    report << "Simulation Duration: " << format_time(simulation_end_time_ - simulation_start_time_) << "\n";
    report << "\n";
    report << "Process Metrics:\n";
    report << "  Total Processes: " << metrics.total_processes << "\n";
    report << "  Completed: " << metrics.completed_processes << "\n";
    report << "  Throughput: " << metrics.throughput << " processes/sec\n";
    report << "  Avg Turnaround Time: " << metrics.average_turnaround_time << " ms\n";
    report << "  Avg Waiting Time: " << metrics.average_waiting_time << " ms\n";
    report << "  Context Switches: " << metrics.context_switches << "\n";
    report << "\n";
    report << "Resource Utilization:\n";
    report << "  CPU Utilization: " << (metrics.cpu_utilization * 100.0) << "%\n";
    report << "  Memory Utilization: " << (metrics.memory_utilization * 100.0) << "%\n";
    report << "  Memory Fragmentation: " << (metrics.fragmentation * 100.0) << "%\n";
    report << "\n";
    report << "Optimization Effectiveness:\n";
    report << "  High throughput indicates efficient scheduling\n";
    report << "  Low fragmentation demonstrates effective memory management\n";
    report << "  Balanced utilization shows optimal resource allocation\n";
    
    return report.str();
}

void ResourceAnalytics::reset() {
    simulation_start_time_ = 0;
    simulation_end_time_ = 0;
}

uint64_t ResourceAnalytics::get_start_time() const noexcept {
    return simulation_start_time_;
}

uint64_t ResourceAnalytics::get_end_time() const noexcept {
    return simulation_end_time_;
}

void ResourceAnalytics::set_time_bounds(uint64_t start, uint64_t end) {
    simulation_start_time_ = start;
    simulation_end_time_ = end;
}

uint64_t ResourceAnalytics::calculate_total_execution_time() const {
    const auto& all_processes = process_manager_.get_all_processes();
    uint64_t total = 0;
    
    for (const auto* process : all_processes) {
        if (process->is_completed()) {
            total += process->get_burst_time();
        }
    }
    
    return total;
}

uint64_t ResourceAnalytics::calculate_total_waiting_time() const {
    const auto& all_processes = process_manager_.get_all_processes();
    uint64_t total = 0;
    
    for (const auto* process : all_processes) {
        if (process->is_completed()) {
            total += process->get_waiting_time();
        }
    }
    
    return total;
}

std::string ResourceAnalytics::format_time(uint64_t milliseconds) const {
    uint64_t seconds = milliseconds / 1000;
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;
    
    std::ostringstream time_str;
    if (hours > 0) {
        time_str << hours << "h " << (minutes % 60) << "m " << (seconds % 60) << "s";
    } else if (minutes > 0) {
        time_str << minutes << "m " << (seconds % 60) << "s";
    } else {
        time_str << seconds << "s";
    }
    
    return time_str.str();
}

} // namespace osro
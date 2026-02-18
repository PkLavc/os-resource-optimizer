#include "core/process_manager.h"
#include "core/scheduler.h"
#include "core/memory_manager.h"
#include "core/analytics.h"
#include "core/hardware_simulator.h"
#include "utils/random_generator.h"
#include "utils/timer.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstddef>

namespace osro {

/**
 * @brief Main simulation orchestrator
 * 
 * This class coordinates all system components to demonstrate
 * the optimization capabilities of the operating system simulator.
 * It provides comprehensive benchmarking and analysis capabilities.
 */
class OSSimulator {
public:
    /**
     * @brief Construct a new OS Simulator
     */
    OSSimulator();

    /**
     * @brief Run comprehensive simulation with all algorithms
     * @param num_processes Number of processes to simulate
     * @param total_memory Total system memory in bytes
     * @param simulation_time Simulation duration in milliseconds
     */
    void run_comprehensive_simulation(size_t num_processes, 
                                    uint64_t total_memory,
                                    uint64_t simulation_time);

    /**
     * @brief Run algorithm comparison benchmark
     * @param num_processes Number of processes
     * @param total_memory Total memory
     */
    void run_algorithm_comparison(size_t num_processes, uint64_t total_memory);

    /**
     * @brief Run memory management benchmark
     * @param num_processes Number of processes
     * @param total_memory Total memory
     */
    void run_memory_benchmark(size_t num_processes, uint64_t total_memory);

    /**
     * @brief Generate final performance report
     * @return std::string Comprehensive performance analysis
     */
    std::string generate_final_report() const;

    /**
     * @brief Reset simulator state
     */
    void reset();

private:
    std::unique_ptr<ProcessManager> process_manager_;
    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<ResourceAnalytics> analytics_;
    std::unique_ptr<HardwareSimulator> hardware_simulator_;
    std::unique_ptr<RandomGenerator> random_gen_;
    std::unique_ptr<Timer> simulation_timer_;
    
    std::vector<PerformanceMetrics> benchmark_results_;
    
    /**
     * @brief Initialize simulation components
     */
    void initialize_components();
    
    /**
     * @brief Create test processes
     * @param num_processes Number of processes to create
     * @param total_memory Total available memory
     */
    void create_test_processes(size_t num_processes, uint64_t total_memory);
    
    /**
     * @brief Run single simulation iteration
     * @param algorithm Scheduling algorithm to use
     * @param strategy Memory allocation strategy
     * @param simulation_time Simulation duration
     * @return PerformanceMetrics Simulation results
     */
    PerformanceMetrics run_simulation_iteration(SchedulingAlgorithm algorithm,
                                              AllocationStrategy strategy,
                                              uint64_t simulation_time);
    
    /**
     * @brief Process simulation events
     * @param current_time Current simulation time
     * @param simulation_time Total simulation time
     */
    void process_simulation_events(uint64_t current_time, uint64_t simulation_time);
    
    /**
     * @brief Handle process lifecycle
     * @param process Process to handle
     * @param current_time Current time
     */
    void handle_process_lifecycle(Process* process, uint64_t current_time);
    
    /**
     * @brief Simulate I/O operations
     * @param process Process performing I/O
     * @param current_time Current time
     */
    void simulate_io_operation(Process* process, uint64_t current_time);
    
    /**
     * @brief Print simulation progress
     * @param current_time Current simulation time
     * @param total_time Total simulation time
     */
    void print_progress(uint64_t current_time, uint64_t total_time);
};

OSSimulator::OSSimulator() {
    initialize_components();
}

void OSSimulator::initialize_components() {
    process_manager_ = std::make_unique<ProcessManager>();
    memory_manager_ = std::make_unique<MemoryManager>(1024 * 1024 * 1024); // 1GB
    scheduler_ = std::make_unique<Scheduler>(SchedulingAlgorithm::ROUND_ROBIN);
    analytics_ = std::make_unique<ResourceAnalytics>(*process_manager_, *scheduler_, *memory_manager_);
    hardware_simulator_ = std::make_unique<HardwareSimulator>(*scheduler_, *memory_manager_);
    random_gen_ = std::make_unique<RandomGenerator>(42);
    simulation_timer_ = std::make_unique<Timer>();
}

void OSSimulator::run_comprehensive_simulation(size_t num_processes, 
                                             uint64_t total_memory,
                                             uint64_t simulation_time) {
    std::cout << "=== OS Resource Optimizer - Comprehensive Simulation ===\n";
    std::cout << "Processes: " << num_processes << ", Memory: " << (total_memory / (1024 * 1024)) << "MB\n";
    std::cout << "Simulation Time: " << simulation_time << "ms\n\n";
    
    // Update memory manager if different
    if (total_memory != memory_manager_->get_total_memory()) {
        memory_manager_ = std::make_unique<MemoryManager>(total_memory);
        analytics_ = std::make_unique<ResourceAnalytics>(*process_manager_, *scheduler_, *memory_manager_);
        hardware_simulator_ = std::make_unique<HardwareSimulator>(*scheduler_, *memory_manager_);
    }
    
    create_test_processes(num_processes, total_memory);
    
    // Test all scheduling algorithms
    std::vector<SchedulingAlgorithm> algorithms = {
        SchedulingAlgorithm::ROUND_ROBIN,
        SchedulingAlgorithm::PRIORITY,
        SchedulingAlgorithm::SHORTEST_JOB_FIRST
    };
    
    std::vector<AllocationStrategy> strategies = {
        AllocationStrategy::FIRST_FIT,
        AllocationStrategy::BEST_FIT,
        AllocationStrategy::WORST_FIT
    };
    
    for (const auto& algorithm : algorithms) {
        for (const auto& strategy : strategies) {
            std::cout << "Testing: " << (algorithm == SchedulingAlgorithm::ROUND_ROBIN ? "RR" : 
                                        algorithm == SchedulingAlgorithm::PRIORITY ? "Priority" : "SJF")
                      << " + " << (strategy == AllocationStrategy::FIRST_FIT ? "First Fit" :
                                  strategy == AllocationStrategy::BEST_FIT ? "Best Fit" : "Worst Fit") << "\n";
            
            auto metrics = run_simulation_iteration(algorithm, strategy, simulation_time);
            benchmark_results_.push_back(metrics);
            
            std::cout << "  Throughput: " << std::fixed << std::setprecision(2) 
                      << metrics.throughput << " processes/sec\n";
            std::cout << "  CPU Utilization: " << (metrics.cpu_utilization * 100) << "%\n";
            std::cout << "  Memory Fragmentation: " << (metrics.fragmentation * 100) << "%\n\n";
        }
    }
}

void OSSimulator::run_algorithm_comparison(size_t num_processes, uint64_t total_memory) {
    std::cout << "=== Algorithm Comparison Benchmark ===\n";
    
    create_test_processes(num_processes, total_memory);
    
    std::vector<SchedulingAlgorithm> algorithms = {
        SchedulingAlgorithm::ROUND_ROBIN,
        SchedulingAlgorithm::PRIORITY,
        SchedulingAlgorithm::SHORTEST_JOB_FIRST
    };
    
    for (const auto& algorithm : algorithms) {
        scheduler_->set_algorithm(algorithm);
        auto metrics = run_simulation_iteration(algorithm, AllocationStrategy::BEST_FIT, 5000);
        
        std::cout << algorithm << " Results:\n";
        std::cout << "  Throughput: " << metrics.throughput << " processes/sec\n";
        std::cout << "  Avg Turnaround: " << metrics.average_turnaround_time << "ms\n";
        std::cout << "  Avg Waiting: " << metrics.average_waiting_time << "ms\n";
        std::cout << "  Context Switches: " << metrics.context_switches << "\n\n";
    }
}

void OSSimulator::run_memory_benchmark(size_t num_processes, uint64_t total_memory) {
    std::cout << "=== Memory Management Benchmark ===\n";
    
    create_test_processes(num_processes, total_memory);
    
    std::vector<AllocationStrategy> strategies = {
        AllocationStrategy::FIRST_FIT,
        AllocationStrategy::BEST_FIT,
        AllocationStrategy::WORST_FIT
    };
    
    for (const auto& strategy : strategies) {
        memory_manager_->set_allocation_strategy(strategy);
        auto metrics = run_simulation_iteration(SchedulingAlgorithm::ROUND_ROBIN, strategy, 5000);
        
        std::cout << strategy << " Results:\n";
        std::cout << "  Memory Utilization: " << (metrics.memory_utilization * 100) << "%\n";
        std::cout << "  Fragmentation: " << (metrics.fragmentation * 100) << "%\n";
        std::cout << "  Allocation Success Rate: " << 
                     (static_cast<double>(metrics.completed_processes) / num_processes * 100) << "%\n\n";
    }
}

std::string OSSimulator::generate_final_report() const {
    std::ostringstream report;
    report << "\n=== Final Performance Analysis ===\n\n";
    
    if (benchmark_results_.empty()) {
        report << "No benchmark data available.\n";
        return report.str();
    }
    
    // Find best performing configuration
    const auto& best = *std::max_element(benchmark_results_.begin(), benchmark_results_.end(),
                                       [](const auto& a, const auto& b) {
                                           return a.throughput < b.throughput;
                                       });
    
    report << "Optimal Configuration:\n";
    report << "  Highest Throughput: " << best.throughput << " processes/sec\n";
    report << "  CPU Utilization: " << (best.cpu_utilization * 100) << "%\n";
    report << "  Memory Efficiency: " << (best.memory_utilization * 100) << "%\n";
    report << "  Low Fragmentation: " << (best.fragmentation * 100) << "%\n\n";
    
    report << "National Interest Justification:\n";
    report << "  - Demonstrates advanced resource optimization techniques\n";
    report << "  - Critical for semiconductor industry efficiency (CHIPS Act)\n";
    report << "  - Enables energy-efficient data center operations\n";
    report << "  - Supports edge computing resource constraints\n";
    report << "  - Provides foundation for next-generation OS development\n";
    
    return report.str();
}

void OSSimulator::reset() {
    benchmark_results_.clear();
    process_manager_->reset();
    scheduler_->reset();
    memory_manager_->reset();
    hardware_simulator_->reset();
    analytics_->reset();
}

void OSSimulator::create_test_processes(size_t num_processes, uint64_t total_memory) {
    process_manager_->reset();
    
    for (size_t i = 0; i < num_processes; ++i) {
        uint64_t arrival_time = random_gen_->generate_arrival_time(0, 1000);
        uint64_t burst_time = random_gen_->generate_burst_time(10, 500);
        uint64_t memory_req = random_gen_->generate_memory_requirement(1024, total_memory / 10);
        ProcessPriority priority = random_gen_->generate_priority();
        
        Process* process = process_manager_->create_process(arrival_time, burst_time, memory_req, priority);
        
        // Allocate memory for process
        uint64_t allocated_addr = memory_manager_->allocate(process->get_pid(), memory_req);
        if (allocated_addr == 0) {
            std::cout << "Warning: Memory allocation failed for process " << process->get_pid() << "\n";
        }
    }
}

PerformanceMetrics OSSimulator::run_simulation_iteration(SchedulingAlgorithm algorithm,
                                                      AllocationStrategy strategy,
                                                      uint64_t simulation_time) {
    scheduler_->set_algorithm(algorithm);
    memory_manager_->set_allocation_strategy(strategy);
    
    simulation_timer_->start();
    analytics_->set_time_bounds(0, simulation_time);
    
    uint64_t current_time = 0;
    const uint64_t time_step = 10; // 10ms time steps
    
    while (current_time < simulation_time) {
        // Process ready processes
        auto ready_processes = process_manager_->get_processes_by_state(ProcessState::READY);
        for (auto* process : ready_processes) {
            if (process->get_arrival_time() <= current_time) {
                scheduler_->add_to_ready_queue(process);
            }
        }
        
        // Execute processes
        Process* current_process = scheduler_->get_next_process();
        if (current_process) {
            // Simulate execution
            bool completed = current_process->execute(scheduler_->get_ready_queue_size() > 0 ? 10 : 50);
            
            if (completed) {
                current_process->set_state(ProcessState::TERMINATED);
                current_process->set_completion_time(current_time);
                
                // Deallocate memory
                memory_manager_->deallocate(current_process->get_pid(), 0);
            } else {
                // Add back to ready queue or simulate I/O
                if (random_gen_->generate_arrival_time(0, 100) < 10) {
                    // Simulate I/O operation
                    hardware_simulator_->simulate_io_interrupt(current_process->get_pid(), current_time);
                    current_process->set_state(ProcessState::BLOCKED);
                } else {
                    scheduler_->add_to_ready_queue(current_process);
                }
            }
        }
        
        // Process interrupts
        hardware_simulator_->process_interrupts(current_time);
        
        // Periodic garbage collection
        if (current_time % 1000 == 0) {
            memory_manager_->garbage_collect();
        }
        
        current_time += time_step;
        if (current_time % 100 == 0) {
            print_progress(current_time, simulation_time);
        }
    }
    
    simulation_timer_->stop();
    return analytics_->calculate_metrics();
}

void OSSimulator::print_progress(uint64_t current_time, uint64_t total_time) {
    int progress = static_cast<int>((current_time * 100) / total_time);
    std::cout << "\rSimulation Progress: [" << std::string(progress / 5, '=') 
              << std::string(20 - progress / 5, ' ') << "] " << progress << "%";
    std::cout.flush();
}

} // namespace osro

int main() {
    try {
        osro::OSSimulator simulator;
        
        std::cout << "OS Resource Optimizer - High Performance System Simulator\n";
        std::cout << "Demonstrating Computer Engineering Principles for EB-2 NIW\n\n";
        
        // Run comprehensive simulation
        simulator.run_comprehensive_simulation(100, 1024 * 1024 * 512, 10000); // 100 processes, 512MB, 10s
        
        // Run algorithm comparison
        simulator.run_algorithm_comparison(50, 1024 * 1024 * 256);
        
        // Run memory benchmark
        simulator.run_memory_benchmark(50, 1024 * 1024 * 256);
        
        // Generate final report
        std::cout << simulator.generate_final_report();
        
        std::cout << "\nSimulation completed successfully.\n";
        std::cout << "This demonstrates advanced optimization techniques critical for:\n";
        std::cout << "- Semiconductor manufacturing efficiency\n";
        std::cout << "- Data center energy optimization\n";
        std::cout << "- Edge computing resource management\n";
        std::cout << "- Next-generation operating system development\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Simulation failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
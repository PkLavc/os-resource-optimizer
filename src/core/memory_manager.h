#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace osro {

/**
 * @brief Enumeration of memory allocation strategies
 */
enum class AllocationStrategy {
    FIRST_FIT,    // Allocate first available block that fits
    BEST_FIT,     // Allocate smallest available block that fits
    WORST_FIT     // Allocate largest available block
};

/**
 * @brief Represents a memory block in the simulated system
 */
struct MemoryBlock {
    uint64_t address;      // Starting address
    uint64_t size;         // Block size in bytes
    bool is_allocated;     // Allocation status
    uint32_t process_id;   // Process ID using this block (0 if free)
    
    MemoryBlock(uint64_t addr, uint64_t sz) 
        : address(addr), size(sz), is_allocated(false), process_id(0) {}
};

/**
 * @brief Simulates a memory management unit with paging/segmentation
 * 
 * This class implements memory allocation strategies and simulates
 * paging with virtual-to-physical address translation. It demonstrates
 * core memory management concepts including fragmentation, garbage
 * collection, and defragmentation.
 */
class MemoryManager {
public:
    /**
     * @brief Construct a new Memory Manager
     * @param total_memory Total system memory in bytes
     * @param page_size Page size for paging simulation
     * @param strategy Allocation strategy to use
     */
    MemoryManager(uint64_t total_memory, 
                  uint64_t page_size = 4096,
                  AllocationStrategy strategy = AllocationStrategy::FIRST_FIT);

    /**
     * @brief Allocate memory for a process
     * @param process_id Process identifier
     * @param size Memory size required
     * @return uint64_t Starting virtual address, 0 if allocation failed
     */
    uint64_t allocate(uint32_t process_id, uint64_t size);

    /**
     * @brief Deallocate memory for a process
     * @param process_id Process identifier
     * @param virtual_address Virtual address to deallocate
     * @return bool True if deallocation successful
     */
    bool deallocate(uint32_t process_id, uint64_t virtual_address);

    /**
     * @brief Get memory usage statistics
     * @return double Memory utilization percentage (0.0 to 1.0)
     */
    double get_utilization() const;

    /**
     * @brief Get fragmentation level
     * @return double Fragmentation percentage (0.0 to 1.0)
     */
    double get_fragmentation() const;

    /**
     * @brief Perform garbage collection/defragmentation
     * @return size_t Number of bytes compacted
     */
    size_t garbage_collect();

    /**
     * @brief Get total memory size
     * @return uint64_t Total memory in bytes
     */
    uint64_t get_total_memory() const noexcept;

    /**
     * @brief Get free memory size
     * @return uint64_t Free memory in bytes
     */
    uint64_t get_free_memory() const noexcept;

    /**
     * @brief Get allocated memory size
     * @return uint64_t Allocated memory in bytes
     */
    uint64_t get_allocated_memory() const noexcept;

    /**
     * @brief Set allocation strategy
     * @param strategy New allocation strategy
     */
    void set_allocation_strategy(AllocationStrategy strategy);

    /**
     * @brief Reset memory manager to initial state
     */
    void reset();

    /**
     * @brief Get memory map for debugging
     * @return std::vector<MemoryBlock> Current memory state
     */
    std::vector<MemoryBlock> get_memory_map() const;

private:
    struct VirtualAddress {
        uint32_t page_number;
        uint64_t offset;
        
        VirtualAddress(uint64_t addr, uint64_t page_size) 
            : page_number(addr / page_size), offset(addr % page_size) {}
    };

    struct PageTableEntry {
        uint64_t physical_address;
        bool valid;
        uint32_t process_id;
        
        PageTableEntry() : physical_address(0), valid(false), process_id(0) {}
    };

    uint64_t total_memory_;
    uint64_t page_size_;
    AllocationStrategy strategy_;
    
    std::vector<MemoryBlock> memory_blocks_;
    std::unordered_map<uint32_t, std::vector<VirtualAddress>> process_allocations_;
    std::vector<PageTableEntry> page_table_;
    
    /**
     * @brief Find best fit block for allocation
     * @param size Required size
     * @return size_t Index of best block, SIZE_MAX if none found
     */
    size_t find_best_fit(uint64_t size) const;

    /**
     * @brief Find first fit block for allocation
     * @param size Required size
     * @return size_t Index of first block, SIZE_MAX if none found
     */
    size_t find_first_fit(uint64_t size) const;

    /**
     * @brief Find worst fit block for allocation
     * @param size Required size
     * @return size_t Index of worst block, SIZE_MAX if none found
     */
    size_t find_worst_fit(uint64_t size) const;

    /**
     * @brief Split a memory block
     * @param block_index Index of block to split
     * @param size Size to allocate
     */
    void split_block(size_t block_index, uint64_t size);

    /**
     * @brief Coalesce adjacent free blocks
     */
    void coalesce_blocks();

    /**
     * @brief Initialize memory blocks
     */
    void initialize_memory();
};

} // namespace osro
#include "memory_manager.h"
#include <algorithm>
#include <numeric>
#include <vector>
#include <stdexcept>
#include <iterator>

namespace osro {

MemoryManager::MemoryManager(uint64_t total_memory, 
                           uint64_t page_size,
                           AllocationStrategy strategy)
    : total_memory_(total_memory),
      page_size_(page_size),
      strategy_(strategy) {
    
    if (total_memory == 0) {
        throw std::invalid_argument("Total memory must be greater than 0");
    }
    
    if (page_size == 0) {
        throw std::invalid_argument("Page size must be greater than 0");
    }
    
    initialize_memory();
}

uint64_t MemoryManager::allocate(uint32_t process_id, uint64_t size) {
    if (size == 0) {
        return 0;
    }
    
    if (size > total_memory_) {
        return 0; // Request exceeds total memory
    }
    
    size_t block_index = SIZE_MAX;
    
    switch (strategy_) {
        case AllocationStrategy::FIRST_FIT:
            block_index = find_first_fit(size);
            break;
        case AllocationStrategy::BEST_FIT:
            block_index = find_best_fit(size);
            break;
        case AllocationStrategy::WORST_FIT:
            block_index = find_worst_fit(size);
            break;
    }
    
    if (block_index == SIZE_MAX) {
        return 0; // No suitable block found
    }
    
    MemoryBlock& block = memory_blocks_[block_index];
    
    // Mark block as allocated
    block.is_allocated = true;
    block.process_id = process_id;
    
    // Split block if necessary
    if (block.size > size) {
        split_block(block_index, size);
    }
    
    // Create virtual address mapping
    VirtualAddress vaddr(block.address, page_size_);
    
    // Update process allocations
    process_allocations_[process_id].push_back(vaddr);
    
    return block.address;
}

bool MemoryManager::deallocate(uint32_t process_id, uint64_t virtual_address) {
    // Find the memory block
    auto it = std::find_if(memory_blocks_.begin(), memory_blocks_.end(),
                          [virtual_address](const MemoryBlock& block) {
                              return block.address == virtual_address && block.is_allocated;
                          });
    
    if (it == memory_blocks_.end()) {
        return false; // Block not found or not allocated
    }
    
    // Mark as free
    it->is_allocated = false;
    it->process_id = 0;
    
    // Remove from process allocations
    auto proc_it = process_allocations_.find(process_id);
    if (proc_it != process_allocations_.end()) {
        auto& allocations = proc_it->second;
        auto alloc_it = std::find_if(allocations.begin(), allocations.end(),
                                   [virtual_address, this](const VirtualAddress& vaddr) {
                                       return (vaddr.page_number * page_size_ + vaddr.offset) == virtual_address;
                                   });
        
        if (alloc_it != allocations.end()) {
            allocations.erase(alloc_it);
        }
        
        if (allocations.empty()) {
            process_allocations_.erase(proc_it);
        }
    }
    
    // Coalesce adjacent free blocks
    coalesce_blocks();
    
    return true;
}

double MemoryManager::get_utilization() const {
    uint64_t allocated = get_allocated_memory();
    return static_cast<double>(allocated) / total_memory_;
}

double MemoryManager::get_fragmentation() const {
    uint64_t total_free = get_free_memory();
    if (total_free == 0) return 0.0;
    
    // Find largest contiguous free block
    uint64_t largest_free = 0;
    for (const auto& block : memory_blocks_) {
        if (!block.is_allocated && block.size > largest_free) {
            largest_free = block.size;
        }
    }
    
    if (largest_free == 0) return 1.0; // Completely fragmented
    
    return 1.0 - static_cast<double>(largest_free) / total_free;
}

size_t MemoryManager::garbage_collect() {
    size_t compacted = 0;
    
    // Simple defragmentation: move all allocated blocks to beginning
    size_t write_index = 0;
    for (size_t i = 0; i < memory_blocks_.size(); ++i) {
        if (memory_blocks_[i].is_allocated) {
            if (write_index != i) {
                // Move block
                memory_blocks_[write_index] = memory_blocks_[i];
                compacted += memory_blocks_[i].size;
            }
            ++write_index;
        }
    }
    
    // Create single free block at the end
    if (write_index < memory_blocks_.size()) {
        uint64_t free_start = (write_index > 0) ? 
            memory_blocks_[write_index - 1].address + memory_blocks_[write_index - 1].size : 0;
        uint64_t free_size = total_memory_ - free_start;
        
        memory_blocks_.resize(write_index + 1);
        memory_blocks_[write_index] = MemoryBlock(free_start, free_size);
        memory_blocks_[write_index].is_allocated = false;
        memory_blocks_[write_index].process_id = 0;
    }
    
    return compacted;
}

uint64_t MemoryManager::get_total_memory() const noexcept {
    return total_memory_;
}

uint64_t MemoryManager::get_free_memory() const noexcept {
    return std::accumulate(memory_blocks_.begin(), memory_blocks_.end(), 0ULL,
                          [](uint64_t sum, const MemoryBlock& block) {
                              return sum + (!block.is_allocated ? block.size : 0);
                          });
}

uint64_t MemoryManager::get_allocated_memory() const noexcept {
    return std::accumulate(memory_blocks_.begin(), memory_blocks_.end(), 0ULL,
                          [](uint64_t sum, const MemoryBlock& block) {
                              return sum + (block.is_allocated ? block.size : 0);
                          });
}

void MemoryManager::set_allocation_strategy(AllocationStrategy strategy) {
    strategy_ = strategy;
}

void MemoryManager::reset() {
    process_allocations_.clear();
    initialize_memory();
}

std::vector<MemoryBlock> MemoryManager::get_memory_map() const {
    return memory_blocks_;
}

size_t MemoryManager::find_best_fit(uint64_t size) const {
    size_t best_index = SIZE_MAX;
    uint64_t best_size = UINT64_MAX;
    
    for (size_t i = 0; i < memory_blocks_.size(); ++i) {
        const auto& block = memory_blocks_[i];
        if (!block.is_allocated && block.size >= size) {
            if (block.size < best_size) {
                best_size = block.size;
                best_index = i;
            }
        }
    }
    
    return best_index;
}

size_t MemoryManager::find_first_fit(uint64_t size) const {
    for (size_t i = 0; i < memory_blocks_.size(); ++i) {
        const auto& block = memory_blocks_[i];
        if (!block.is_allocated && block.size >= size) {
            return i;
        }
    }
    return SIZE_MAX;
}

size_t MemoryManager::find_worst_fit(uint64_t size) const {
    size_t worst_index = SIZE_MAX;
    uint64_t worst_size = 0;
    
    for (size_t i = 0; i < memory_blocks_.size(); ++i) {
        const auto& block = memory_blocks_[i];
        if (!block.is_allocated && block.size >= size) {
            if (block.size > worst_size) {
                worst_size = block.size;
                worst_index = i;
            }
        }
    }
    
    return worst_index;
}

void MemoryManager::split_block(size_t block_index, uint64_t size) {
    MemoryBlock& block = memory_blocks_[block_index];
    uint64_t remaining_size = block.size - size;
    
    if (remaining_size < page_size_) {
        // Don't split if remaining would be too small
        return;
    }
    
    // Create new block for remaining space
    MemoryBlock new_block(block.address + size, remaining_size);
    
    // Update current block
    block.size = size;
    
    // Insert new block
    memory_blocks_.insert(memory_blocks_.begin() + block_index + 1, new_block);
}

void MemoryManager::coalesce_blocks() {
    for (size_t i = 0; i < memory_blocks_.size() - 1; ) {
        MemoryBlock& current = memory_blocks_[i];
        MemoryBlock& next = memory_blocks_[i + 1];
        
        if (!current.is_allocated && !next.is_allocated) {
            // Merge adjacent free blocks
            current.size += next.size;
            memory_blocks_.erase(memory_blocks_.begin() + i + 1);
        } else {
            ++i;
        }
    }
}

void MemoryManager::initialize_memory() {
    memory_blocks_.clear();
    memory_blocks_.emplace_back(0, total_memory_);
    memory_blocks_[0].is_allocated = false;
    memory_blocks_[0].process_id = 0;
}

} // namespace osro
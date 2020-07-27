#pragma once

#include <cstdint>
#include "Paging.h"

namespace MemoryManager {

/**
 * The physical allocator is a really simple bitmap of 
 * all the possible pages. Its wasteful, yet efficient and simple. 
 * It uses about 130kb for the 1048576 pages that are possible with
 * x86 
 */

#define ALLOCATION_BYTES (MAX_NUM_PHYSICAL_PAGES / 8) 


struct MemoryManagerData
{
    std::uint32_t TotalMemory;
    std::uint32_t UsedMemory;
    std::uint8_t PhysicalAllocation[ALLOCATION_BYTES];
};

/**
 * @brief Initializes the physical allocator
 * 
 * @param multiboot multiboot structure containing the memmory map
 * and other goodiesx   
 */
void physicalAllocatorInit(Multiboot::Multiboot& multiboot);

/**
 * @brief Allocates physical memory at aspecified adddress
 * 
 * @param startAddress the address to start allocation from
 * @param numPages the number of pages to allocate
 */
void physicalAllocate(uint32_t startAddress, std::uint32_t numPages);

/**
 * @brief Allocates a number of pages at the next free location
 * 
 * @param numPages the number of pages to allocate
 * @return uint32_t the physical address at which the pages were allocated
 */
uint32_t physicalAllocate(std::uint32_t numPages);

/**
 * @brief Frees physical pages at the specified address
 * 
 * @param startAddress the address to start freeing at 
 * @param numPages the number of pages to free 
 */
void physicalFree(uint32_t startAddress, uint32_t numPages);

}

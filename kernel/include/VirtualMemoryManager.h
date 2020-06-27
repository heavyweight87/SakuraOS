#pragma once

#include "MemoryManager.h"

namespace MemoryManager {

/**
 * @brief returns true if a page is present
 * 
 * @param pageDirectory  the page directory to use
 * @param virtualAddress the virtual address of the page
 * @return true if the page is present
 */
bool IsPagePresent(PageDirectory& pageDirectory, uint32_t virtualAddress);

/**
 * @brief Get the physical address of a given virtual address
 * 
 * @param pageDirectory the pave directory to use 
 * @param virtualAddress the virtual address to convert
 * @return uint32_t the physical address
 */
uint32_t GetPhysicalAddress(PageDirectory& pageDirectory, uint32_t virtualAddress);

/**
 * @brief Allocatesa number of 
 * 
 * @param pageDirectory 
 * @param physicalAddress 
 * @param numPages 
 * @param user 
 * @return uint32_t 
 */
uint32_t VirtualAllocate(PageDirectory& pageDirectory, uint32_t physicalAddress, uint32_t numPages, bool user);

/**
 * @brief Frees up virtual memory
 * 
 * @param pageDirectory the page directory to use
 * @param virtualAddress the address of the virtual memory to free 
 * @param numPages the number of pages to free 
 */
void VirtualFree(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t numPages);

/**
 * @brief Maps memory so the physical and virtual addresses are the same
 * 
 * @param pageDirectory the page directory to use
 * @param address the start address to use
 * @param size how much memory to map
 */
void IdentityMap(PageDirectory& pageDirectory, uintptr_t address, size_t size);

}
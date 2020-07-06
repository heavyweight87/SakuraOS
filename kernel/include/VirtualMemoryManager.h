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
 * @brief maps a physical address to a virtual one
 * 
 * @param pageDirectory page directory to use
 * @param virtualAddress the virtual address to map 
 * @param physicalAddress the physical address to map
 * @param numPages the number of pages to map
 * @param isUser true if we are mapping user memory
 */
void VirtualMap(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t numPages, bool isUser);

/**
 * @brief Frees up virtual memory
 * 
 * @param pageDirectory the page directory to use
 * @param virtualAddress the address of the virtual memory to free 
 * @param numPages the number of pages to free 
 */
void VirtualFree(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t numPages);

}
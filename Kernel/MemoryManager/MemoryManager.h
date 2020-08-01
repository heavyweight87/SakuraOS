#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cstddef>
#include "Multiboot.h"
#include "Paging.h"

namespace MemoryManager {

/**
 * @brief Initializes memory management and enables paging
 * 
 * @param multiboot structure with multiboot info
 */
void init(Kernel::Multiboot& multiboot);

/**
 * @brief Allocates page alligned memory
 * 
 * @param size how much memory to allocate
 * @param user true if its user memory
 * @return uintptr_t a pointer to the allocated memory
 */
uintptr_t memoryAllocate(std::size_t size, bool user);

/**
 * @brief Allocates page alligned memory
 * 
 * @param pageDirectory page directory from where the memory should be mapped to
 * @param size how much memory to allocate
 * @param user true if its user memory
 * @return uintptr_t a pointer to the allocated memory
 */
uintptr_t memoryAllocate(PageDirectory& pageDirectory, std::size_t size, bool user);

/**
 * @brief Frees allocated memory
 * 
 * @param pageDirectory the page directory of where the memory was mapped to
 * @param startAddress the start address of the memory 
 * @param size how much memory to free
 */
void memoryFree(PageDirectory& pageDirectory, uint32_t startAddress, std::size_t size);
void memoryFree(int32_t startAddress, size_t size);

/**
 * @brief Maps memory so the physical and virtual addresses are the same
 * 
 * @param pageDirectory the page directory to use
 * @param address the start address to use
 * @param size how much memory to map
 */
void identityMap(PageDirectory& pageDirectory, uintptr_t address, std::size_t size);

void memoryMap(PageDirectory& pageDirectory, uint32_t virtualAddress, std::size_t size, bool isUser);

PageDirectory *createUserPageDirectory();

/**
 * @brief returns the kernel page directory
 * 
 * @return PageDirectory& reference to the kernel page directory
 */
PageDirectory& getKerkelPageDirectory();

}
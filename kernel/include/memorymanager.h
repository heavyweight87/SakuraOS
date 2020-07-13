#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cstddef>
#include "multiboot.h"
#include "paging.h"

namespace MemoryManager {

#define TOTAL_MEMORY        (0xFFFFFFFFUL)
#define KERNEL_TOTAL_MEMORY (0x40000000UL)
#define KERNEL_START_ADDRESS (0x100000)

/**
 * @brief Initializes memory management and enables paging
 * 
 * @param multiboot structure with multiboot info
 */
void Init(Multiboot::Multiboot& multiboot);

/**
 * @brief Allocates page alligned memory
 * 
 * @param size how much memory to allocate
 * @param user true if its user memory
 * @return uintptr_t a pointer to the allocated memory
 */
uintptr_t MemoryAllocate(std::size_t size, bool user);

/**
 * @brief Allocates page alligned memory
 * 
 * @param pageDirectory page directory from where the memory should be mapped to
 * @param size how much memory to allocate
 * @param user true if its user memory
 * @return uintptr_t a pointer to the allocated memory
 */
uintptr_t MemoryAllocate(PageDirectory& pageDirectory, std::size_t size, bool user);

/**
 * @brief Frees allocated memory
 * 
 * @param pageDirectory the page directory of where the memory was mapped to
 * @param startAddress the start address of the memory 
 * @param size how much memory to free
 */
void MemoryFree(PageDirectory& pageDirectory, uint32_t startAddress, std::size_t size);
void MemoryFree(int32_t startAddress, size_t size);

/**
 * @brief Maps memory so the physical and virtual addresses are the same
 * 
 * @param pageDirectory the page directory to use
 * @param address the start address to use
 * @param size how much memory to map
 */
void IdentityMap(PageDirectory& pageDirectory, uintptr_t address, std::size_t size);

void MemoryMap(PageDirectory& pageDirectory, uint32_t virtualAddress, std::size_t size, bool isUser);

PageDirectory *CreateUserPageDirectory();

/**
 * @brief returns the kernel page directory
 * 
 * @return PageDirectory& reference to the kernel page directory
 */
PageDirectory& GetKerkelPageDirectory();

}
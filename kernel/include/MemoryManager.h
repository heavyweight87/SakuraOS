#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Multiboot.h"
#include "Paging.h"

namespace MemoryManager {

#define TOTAL_MEMORY        (0xFFFFFFFFUL)
#define KERNEL_TOTAL_MEMORY (0x40000000UL)
#define KERNEL_START_ADDRESS (0x100000)

uintptr_t memory_alloc_identity_page(PageDirectory *pdir);

void Init(Multiboot::Multiboot& multiboot);
void memory_pdir_switch(PageDirectory *pdir);
PageDirectory& memory_kpdir();
uintptr_t MemoryAllocate(PageDirectory& page_directory, size_t size, bool user);
void MemoryFree(PageDirectory& pageDirectory, uint32_t physicalAddress, size_t size);

}
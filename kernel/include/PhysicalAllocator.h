#pragma once

#include <cstdint>

namespace MemoryManager {


#define ALLOCATION_BYTES (MAX_NUM_PHYSICAL_PAGES / 8)


struct MemoryManagerData
{
    std::uint32_t TotalMemory;
    std::uint32_t UsedMemory;
    std::uint8_t PhysicalAllocation[ALLOCATION_BYTES];
};

void InitPhysicalAllocator(Multiboot::Multiboot& multiboot);
uint32_t physical_alloc(uint32_t count);
int physical_is_used(uint32_t addr, uint32_t count);
void physical_set_used(uint32_t addr, uint32_t caount);
void FreePhysical(uint32_t addr, uint32_t count);

}

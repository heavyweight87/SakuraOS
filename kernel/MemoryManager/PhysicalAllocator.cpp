
#include <Paging.h>
#include <PhysicalAllocator.h>
#include "stdio.h"
#include "Multiboot.h"
#include <string.h>

namespace MemoryManager {

MemoryManagerData memManData;

#define PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress)  (memManData.PhysicalAllocation[(uint32_t)(pageAddress) / PAGE_SIZE / 8])

#define PHYSICAL_ALLOCATOR_IS_USED(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) & (1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))
#define PHYSICAL_ALLOCATOR_IS_PAGE_FREE(pageAddress) !PHYSICAL_ALLOCATOR_IS_USED(pageAddress)
#define PHYSICAL_ALLOCATOR_SET(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) |= (1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))
#define PHYSICAL_ALLOCATOR_CLEAR(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) &= ~(1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))

void PhysicalAllocatorInit(Multiboot::Multiboot& multiboot)
{
    memManData.UsedMemory = 0;
    memset(&memManData.PhysicalAllocation, 0xFF, ALLOCATION_BYTES);
    memManData.TotalMemory = multiboot.LoadMemoryMap();

    printf("Total physical memory: %dmb\r\n", memManData.TotalMemory / 1024 /1024 );
}

int physical_is_used(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (PHYSICAL_ALLOCATOR_IS_USED(addr + (i * PAGE_SIZE)))
            return 1;
    }

    return 0;
}

uint32_t PhysicalAllocate(uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < (memManData.TotalMemory / PAGE_SIZE); pageIndex++)
    {
        uint32_t physicalAddress = pageIndex * PAGE_SIZE;
        if (!physical_is_used(physicalAddress, numPages))
        {
            PhysicalAllocate(physicalAddress, numPages);
            return physicalAddress;
        }
    }

    printf("Out of memory! Trying to allocated %d pages but we hae only % left", numPages, (memManData.TotalMemory - memManData.UsedMemory) / PAGE_SIZE);
    return 0;
}

void PhysicalAllocate(uint32_t startAddress, uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < numPages; pageIndex++)
    {
        if (!PHYSICAL_ALLOCATOR_IS_USED(startAddress + (pageIndex * PAGE_SIZE)))
        {
            memManData.UsedMemory += PAGE_SIZE;
            PHYSICAL_ALLOCATOR_SET(startAddress + (pageIndex * PAGE_SIZE));
        }
    }
}

void PhysicalFree(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (PHYSICAL_ALLOCATOR_IS_USED(addr + (i * PAGE_SIZE)))
        {
            memManData.UsedMemory -= PAGE_SIZE;
            PHYSICAL_ALLOCATOR_CLEAR(addr + (i * PAGE_SIZE));
        }
    }
}

}
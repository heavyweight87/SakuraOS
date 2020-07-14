
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include "Libk.h"
#include "Multiboot.h"

namespace MemoryManager {

MemoryManagerData memManData;

#define PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress)  (memManData.PhysicalAllocation[(uint32_t)(pageAddress) / PAGE_SIZE / 8])

#define PHYSICAL_ALLOCATOR_IS_USED(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) & (1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))
#define PHYSICAL_ALLOCATOR_IS_PAGE_FREE(pageAddress) !PHYSICAL_ALLOCATOR_IS_USED(pageAddress)
#define PHYSICAL_ALLOCATOR_SET(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) |= (1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))
#define PHYSICAL_ALLOCATOR_CLEAR(pageAddress) (PHYSICAL_ALLOCATOR_GET_BYTE(pageAddress) &= ~(1 << ((uint32_t)(pageAddress) / PAGE_SIZE % 8)))

void physicalAllocatorInit(Multiboot::Multiboot& multiboot)
{
    memManData.UsedMemory = 0;
    Libk::memset(&memManData.PhysicalAllocation, 0xFF, ALLOCATION_BYTES);
    memManData.TotalMemory = multiboot.loadMemoryMap();

    Libk::printk("Total physical memory: %dmb\r\n", memManData.TotalMemory / 1024 /1024 );
}

static bool isPhysicalFree(uint32_t startAddress, uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < numPages; pageIndex++)
    {
        uint32_t address = startAddress + (pageIndex * PAGE_SIZE);
        if (PHYSICAL_ALLOCATOR_IS_USED(address))
        {
            return false;
        }
    }
    return true;
}

uint32_t physicalAllocate(std::uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < (memManData.TotalMemory / PAGE_SIZE); pageIndex++)
    {
        uint32_t physicalAddress = pageIndex * PAGE_SIZE;
        if (isPhysicalFree(physicalAddress, numPages))
        {
            physicalAllocate(physicalAddress, numPages);
            return physicalAddress;
        }
    }
    Libk::printk("Out of physical memory! Trying to allocated %d pages but we hae only %d left\r\n", numPages, (memManData.TotalMemory - memManData.UsedMemory) / PAGE_SIZE);
    return 0;
}

void physicalAllocate(uint32_t startAddress, std::uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < numPages; pageIndex++)
    {
        uint32_t address = startAddress + (pageIndex * PAGE_SIZE);
        if (PHYSICAL_ALLOCATOR_IS_USED(address) == false)
        {
            memManData.UsedMemory += PAGE_SIZE;
            PHYSICAL_ALLOCATOR_SET(address);
        }
        else
        {
            Libk::printk("Physical allocate failed, address already used\r\n");
        }
    }
}

void physicalFree(uint32_t startAddress, uint32_t numPages)
{
    for (uint32_t pageIndex = 0; pageIndex < numPages; pageIndex++)
    {
        uint32_t address = startAddress + (pageIndex * PAGE_SIZE);
        if (PHYSICAL_ALLOCATOR_IS_USED(address))
        {
            memManData.UsedMemory -= PAGE_SIZE;
            PHYSICAL_ALLOCATOR_CLEAR(address);
        }
    }
}

}
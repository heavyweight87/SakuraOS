
#include <Paging.h>
#include <PhysicalAllocator.h>
#include "stdio.h"
#include "Multiboot.h"

namespace MemoryManager {

MemoryManagerData memManData;


#define PHYSICAL_IS_USED(__addr) \
    (memManData.PhysicalAllocation[(uint32_t)(__addr) / PAGE_SIZE / 8] & (1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_USED(__addr) \
    (memManData.PhysicalAllocation[(uint32_t)(__addr) / PAGE_SIZE / 8] |= (1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_FREE(__addr) \
    (memManData.PhysicalAllocation[(uint32_t)(__addr) / PAGE_SIZE / 8] &= ~(1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

void InitPhysicalAllocator(Multiboot::Multiboot& multiboot)
{
    printf("pp = %d", MAX_NUM_PHYSICAL_PAGES);
    for (size_t i = 0; i < 1024 * 1024 / 8; i++)
    {
        memManData.PhysicalAllocation[i] = 0xff;
    }
    memManData.UsedMemory = 0;
    memManData.TotalMemory = multiboot.LoadMemoryMap();

    printf("Total physical memory: %dmb\r\n", memManData.TotalMemory / 1024 /1024 );
}

int physical_is_used(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
            return 1;
    }

    return 0;
}

void physical_set_used(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (!PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
        {
            memManData.UsedMemory += PAGE_SIZE;
            PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
        }
    }
}

void FreePhysical(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
        {
            memManData.UsedMemory -= PAGE_SIZE;
            PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
        }
    }
}

uint32_t physical_alloc(uint32_t count)
{
    for (uint32_t i = 0; i < (memManData.TotalMemory / PAGE_SIZE); i++)
    {
        uint32_t addr = i * PAGE_SIZE;
        if (!physical_is_used(addr, count))
        {
            physical_set_used(addr, count);
            return addr;
        }
    }

    printf("Out of physical memory!\n\tTrying to allocat %d pages but free memory is %d pages !", count, (memManData.TotalMemory - memManData.UsedMemory) / PAGE_SIZE);
    return 0;
}

}
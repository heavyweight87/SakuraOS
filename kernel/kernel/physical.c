
#include "kernel/physical.h"
#include "kernel/paging.h"
#include "stdio.h"

uint32_t TOTAL_MEMORY = 0;
uint32_t USED_MEMORY = 0;

uint32_t FREE_MEMORY_SHORTCUT = 0;
uint8_t MEMORY[1024 * 1024 / 8] = {};

#define PHYSICAL_IS_USED(__addr) \
    (MEMORY[(uint32_t)(__addr) / PAGE_SIZE / 8] & (1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_USED(__addr) \
    (MEMORY[(uint32_t)(__addr) / PAGE_SIZE / 8] |= (1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

#define PHYSICAL_SET_FREE(__addr) \
    (MEMORY[(uint32_t)(__addr) / PAGE_SIZE / 8] &= ~(1 << ((uint32_t)(__addr) / PAGE_SIZE % 8)))

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
            USED_MEMORY += PAGE_SIZE;
            PHYSICAL_SET_USED(addr + (i * PAGE_SIZE));
        }
    }
}

void physical_set_free(uint32_t addr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        if (PHYSICAL_IS_USED(addr + (i * PAGE_SIZE)))
        {
            USED_MEMORY -= PAGE_SIZE;
            PHYSICAL_SET_FREE(addr + (i * PAGE_SIZE));
        }
    }
}

uint32_t physical_alloc(uint32_t count)
{
    for (uint32_t i = 0; i < (TOTAL_MEMORY / PAGE_SIZE); i++)
    {
        uint32_t addr = i * PAGE_SIZE;
        if (!physical_is_used(addr, count))
        {
            physical_set_used(addr, count);
            return addr;
        }
    }

    printf("Out of physical memory!\n\tTrying to allocat %d pages but free memory is %d pages !", count, (TOTAL_MEMORY - USED_MEMORY) / PAGE_SIZE);
    return 0;
}

void physical_free(uint32_t addr, uint32_t count)
{
    physical_set_free(addr, count);
}

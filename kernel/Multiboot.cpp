#include <vector>
#include "Multiboot.h"
#include "Paging.h"
#include "PhysicalAllocator.h"

namespace Multiboot {
    
Multiboot::Multiboot(MultibootInfo& multibootInfo)  : m_multibootInfo(multibootInfo)
{
    LoadModules();
}

void Multiboot::LoadModules()
{
    typedef void (*callModule)(void);
    std::uint32_t moduleNum = 0;
    if(m_multibootInfo.modsCount > 0)
    {
        ModuleInfo *module = (ModuleInfo*)m_multibootInfo.modsAddress;
        while(moduleNum < m_multibootInfo.modsCount)
        {
            callModule start_program = (callModule) module->modStart;
            module++;
            start_program();
        }
    }
}

std::uint32_t Multiboot::LoadMemoryMap()
{
    std::uint32_t totalFreeMemory = 0;
    MemoryMap *map = (MemoryMap*)m_multibootInfo.mmapAddress;
    while((uint64_t)map < m_multibootInfo.mmapAddress + m_multibootInfo.mmapLength)
    {
        std::uint32_t base = map->address;
        std::uint32_t size = map->length;
        if(map->type == 1)
        {
          //  MemoryRange range = memory_range_around_non_aligned_address(map->address, map->length);
            size_t align = PAGE_SIZE - base % PAGE_SIZE;

            if (base % PAGE_SIZE == 0)
            {
                align = 0;
            }

            base += align;
            size -= align;

            size -= size % PAGE_SIZE;
            MemoryManager::FreePhysical(base, size / PAGE_SIZE);
            totalFreeMemory += map->length;
        }
        
        map = (MemoryMap*)((uint64_t)map + map->size + sizeof(map->size));
    }
    return totalFreeMemory;
}

}
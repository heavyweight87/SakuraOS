#include <vector>
#include "Multiboot.h"
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include <stdio.h>
#include <string.h>

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
        printf("Loading %d modules\r\n", m_multibootInfo.modsCount);
        ModuleInfo *module = (ModuleInfo*)m_multibootInfo.modsAddress;
        char str[20];
        memcpy(str, (char*)module->string, 20);
        printf("string = %s\r\n", str);
        while(moduleNum < m_multibootInfo.modsCount)
        {
            callModule start_program = (callModule) module->modStart;
            start_program();
            module++;
        }
    }
}

std::uint32_t Multiboot::LoadMemoryMap()
{
    std::uint32_t totalFreeMemory = 0;
    MemoryMap *map = (MemoryMap*)m_multibootInfo.mmapAddress;
    while((uint64_t)map < (m_multibootInfo.mmapAddress + m_multibootInfo.mmapLength))
    {
        std::uint32_t base = map->address;
        std::uint32_t size = map->length;
        if(map->type == 1)
        {
            std::uint32_t startAddress = map->address;
            std::uint32_t length = map->length;

            /**
             * If the address is not page aligned then we need to align up to the
             * next page. 
            */
            if (startAddress % PAGE_SIZE > 0)
            {
                uint32_t alignUp = PAGE_SIZE - base % PAGE_SIZE;
                startAddress += alignUp;
                length -= alignUp;
            }
            length -= size % PAGE_SIZE; //make sure the last page is aligned down
            MemoryManager::PhysicalFree(startAddress, length / PAGE_SIZE);
            totalFreeMemory += length;
        }
        
        map = (MemoryMap*)((uint64_t)map + map->size + sizeof(map->size));
    }
    return totalFreeMemory;
}

}
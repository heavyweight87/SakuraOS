#include "Multiboot.h"
#include "PhysicalAllocator.h"
#include "Libk.h"
#include "VirtualMemoryManager.h"
#include "Scheduler.h"
#include "Elf.h"

namespace Kernel {

void Multiboot::loadModules()
{
    std::uint32_t moduleNum = 0;
    if(m_multibootInfo.modsCount > 0)
    {
        Libk::printk("Loading %d modules\r\n", m_multibootInfo.modsCount);
        ModuleInfo *module = (ModuleInfo*)m_multibootInfo.modsAddress;
        while(moduleNum < m_multibootInfo.modsCount)
        {   
            uint32_t size = module->modEnd - module->modStart;
            MemoryManager::identityMap(MemoryManager::getKerkelPageDirectory(), module->modStart, size);
            Kernel::load(reinterpret_cast<uint8_t*>(module->modStart), size);
            module++;
            moduleNum++;
        }
    }
}

void Multiboot::reserveModuleMemory()
{
    std::uint32_t moduleNum = 0;
    ModuleInfo *module = (ModuleInfo*)m_multibootInfo.modsAddress;
    while(moduleNum < m_multibootInfo.modsCount)
    {   
        uint32_t size = module->modEnd - module->modStart;
        MemoryManager::physicalFree(module->modStart, size / PAGE_SIZE);
        MemoryManager::identityMap(MemoryManager::getKerkelPageDirectory(), module->modStart, size);
        module++;
        moduleNum++;
    }
}

std::uint32_t Multiboot::loadMemoryMap()
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
            MemoryManager::physicalFree(startAddress, length / PAGE_SIZE);
            totalFreeMemory += length;
        }
        
        map = (MemoryMap*)((uint64_t)map + map->size + sizeof(map->size));
    }
    return totalFreeMemory;
}

}
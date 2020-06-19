#include <vector>
#include "Multiboot.h"

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

}
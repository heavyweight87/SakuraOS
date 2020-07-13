#include "elf.h"
#include "scheduler.h"
#include "libk.h"

namespace Elf {

static bool verifyElfHeader(ElfHeader *header)
{
	if(!header) return false;
	if(header->m_ident[EI_MAG0] != ELFMAG0) 
    {
		Libk::printk("ELF Header EI_MAG0 incorrect.\n");
		return false;
	}
	if(header->m_ident[EI_MAG1] != ELFMAG1) 
    {
		Libk::printk("ELF Header EI_MAG1 incorrect.\n");
		return false;
	}
	if(header->m_ident[EI_MAG2] != ELFMAG2) 
    {
		Libk::printk("ELF Header EI_MAG2 incorrect.\n");
		return false;
	}
	if(header->m_ident[EI_MAG3] != ELFMAG3) 
    {
		Libk::printk("ELF Header EI_MAG3 incorrect.\n");
		return false;
	}
	if(header->m_ident[EI_CLASS] != ELFCLASS32) 
    {
		Libk::printk("Unsupported ELF File Class.\n");
		return false;
	}
	if(header->m_ident[EI_DATA] != ELFDATA2LSB) 
    {
		Libk::printk("Unsupported ELF File byte order.\n");
		return false;
	}
	if(header->m_machine != EM_386) 
    {
		Libk::printk("Unsupported ELF File target.\n");
		return false;
	}
	if(header->m_ident[EI_VERSION] != EV_CURRENT) 
    {
		Libk::printk("Unsupported ELF File version.\n");
		return false;
	}
	if(header->m_type != ET_REL && header->m_type != ET_EXEC) 
    {
		Libk::printk("Unsupported ELF File type.\n");
		return false;
	}
	return true;
}

bool Load(std::uint8_t* elfData, std::uint32_t length)
{
    ElfHeader *header = (ElfHeader*)elfData;
    static Scheduler::Task task;
    Scheduler::createTask(task, 0, false);
    MemoryManager::IdentityMap(*(MemoryManager::PageDirectory*) task.regs.cr3, reinterpret_cast<std::uintptr_t>(elfData), length);
    if(verifyElfHeader(header))
    {
        for(int sec = 0; sec < header->m_phnum; sec++)
        {
            ElfProgramHeader *programHeader = (ElfProgramHeader*)(elfData + header->m_phoff + (header->m_phentsize * sec));         
            programHeader->m_vaddr = programHeader->m_vaddr - (programHeader->m_vaddr % PAGE_SIZE);   	
            MemoryManager::MemoryMap(*(MemoryManager::PageDirectory*) task.regs.cr3, programHeader->m_vaddr, programHeader->m_filesz, false);
            uint8_t *progData = (uint8_t*)(elfData + programHeader->m_offset);
            Libk::memcpy((void*)programHeader->m_vaddr, progData, programHeader->m_filesz);
        }
        Scheduler::TaskEntry entry = reinterpret_cast<Scheduler::TaskEntry>(header->m_entry);
        Scheduler::taskStart(task, entry);
    }    
    return false;    
}

}

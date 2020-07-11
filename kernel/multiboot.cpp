#include <vector>
#include "multiboot.h"
#include "memorymanager.h"
#include "physicalallocator.h"
#include "kstdio.h"
#include "kstring.h"
#include "virtualmemorymanager.h"
#include "scheduler.h"

namespace Multiboot {

    
Multiboot::Multiboot(MultibootInfo& multibootInfo)  : m_multibootInfo(multibootInfo)
{
}


# define ELF_NIDENT	16

typedef struct{
	uint8_t		e_ident[ELF_NIDENT];
	uint16_t	e_type;
	uint16_t	e_machine;
	uint32_t	e_version;
	uint32_t	e_entry;
	uint32_t	e_phoff;
	uint32_t	e_shoff;
	uint32_t	e_flags;
	uint16_t	e_ehsize;
	uint16_t	e_phentsize;
	uint16_t	e_phnum;
	uint16_t	e_shentsize;
	uint16_t	e_shnum;
	uint16_t	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	uint32_t		p_type;
	uint32_t		p_offset;
	uint32_t		p_vaddr;
	uint32_t    	p_paddr;
	uint16_t		p_filesz;
	uint16_t		p_memsz;
	uint16_t		p_flags;
	uint16_t		p_align;
} Elf32_Phdr;

enum Elf_Ident {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
	EI_CLASS	= 4, // Architecture (32/64)
	EI_DATA		= 5, // Byte Order
	EI_VERSION	= 6, // ELF Version
	EI_OSABI	= 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		= 9  // Padding
};
 
# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};
 
# define EM_386		(3)  // x86 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version

bool elf_check_file(Elf32_Ehdr *hdr) {
	if(!hdr) return false;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		Libk::printk("ELF Header EI_MAG0 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		Libk::printk("ELF Header EI_MAG1 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		Libk::printk("ELF Header EI_MAG2 incorrect.\n");
		return false;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		Libk::printk("ELF Header EI_MAG3 incorrect.\n");
		return false;
	}
	return true;
}

bool elf_check_supported(Elf32_Ehdr *hdr) {
	if(!elf_check_file(hdr)) {
		Libk::printk("Invalid ELF File.\n");
		return false;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		Libk::printk("Unsupported ELF File Class.\n");
		return false;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		Libk::printk("Unsupported ELF File byte order.\n");
		return false;
	}
	if(hdr->e_machine != EM_386) {
		Libk::printk("Unsupported ELF File target.\n");
		return false;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		Libk::printk("Unsupported ELF File version.\n");
		return false;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		Libk::printk("Unsupported ELF File type.\n");
		return false;
	}
	return true;
}

void Multiboot::LoadModules()
{
    typedef void (*callModule)(void);
    std::uint32_t moduleNum = 0;
    if(m_multibootInfo.modsCount > 0)
    {
        Libk::printk("Loading %d modules\r\n", m_multibootInfo.modsCount);
        ModuleInfo *module = (ModuleInfo*)m_multibootInfo.modsAddress;
        while(moduleNum < m_multibootInfo.modsCount)
        {   
            uint32_t size = module->modEnd - module->modStart;
            MemoryManager::IdentityMap(MemoryManager::GetKerkelPageDirectory(), module->modStart, size);
            Elf32_Ehdr *hdr = (Elf32_Ehdr *)module->modStart;
            if(elf_check_supported(hdr))
            {
                for(int sec = 0; sec < hdr->e_phnum; sec++)
                {
                    Elf32_Phdr *programHeader = (Elf32_Phdr*)(module->modStart + hdr->e_phoff + (hdr->e_phentsize * sec));         
					programHeader->p_vaddr = programHeader->p_vaddr - (programHeader->p_vaddr % PAGE_SIZE);   	
                    MemoryManager::MemoryMap(MemoryManager::GetKerkelPageDirectory(), programHeader->p_vaddr, programHeader->p_filesz, false);
                    uint8_t *progData = (uint8_t*)(module->modStart + programHeader->p_offset);
                    Libk::memcpy((void*)programHeader->p_vaddr, progData, programHeader->p_filesz);
                }

            }        
            callModule start_program = (callModule)hdr->e_entry;
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
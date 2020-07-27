#pragma once 

#include <cstdint>

namespace Elf {

# define ELF_NIDENT_LENTH	16
# define ELFMAG0	        0x7F // e_ident[EI_MAG0]
# define ELFMAG1	        'E'  // e_ident[EI_MAG1]
# define ELFMAG2	        'L'  // e_ident[EI_MAG2]
# define ELFMAG3	        'F'  // e_ident[EI_MAG3]

enum ElfIdent 
{
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


enum ElfType 
{
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};
 
# define ELFDATA2LSB	    (1)  // Little Endian
# define ELFCLASS32	        (1)  // 32-bit Architecture
# define EM_386		        (3)  // x86 Machine Type
# define EV_CURRENT	        (1)  // ELF Current Version

struct ElfHeader
{
	std::uint8_t	m_ident[ELF_NIDENT_LENTH];
	std::uint16_t	m_type;
	std::uint16_t	m_machine;
	std::uint32_t	m_version;
	std::uint32_t	m_entry;
	std::uint32_t	m_phoff;
	std::uint32_t	m_shoff;
	std::uint32_t	m_flags;
	std::uint16_t	m_ehsize;
	std::uint16_t	m_phentsize;
	std::uint16_t	m_phnum;
	std::uint16_t	m_shentsize;
	std::uint16_t	m_shnum;
	std::uint16_t	m_shstrndx;
};


struct ElfProgramHeader
{
	std::uint32_t		m_type;
	std::uint32_t		m_offset;
	std::uint32_t		m_vaddr;
	std::uint32_t    	m_paddr;
	std::uint16_t		m_filesz;
	std::uint16_t		m_memsz;
	std::uint16_t		m_flags;
	std::uint16_t		m_align;
};

bool load(std::uint8_t* elfData, std::uint32_t length);

}
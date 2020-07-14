#pragma once

#include <cstdint>
// The following was copied from https://www.gnu.org/software/grub/manual/multiboot/html_node/multiboot.h.html

namespace Multiboot {


/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

/* C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/* Types. */

/* The Multiboot header. */
struct MultibootHeader
{
  std::uint32_t magic;
  std::uint32_t flags;
  std::uint32_t checksum;
  std::uint32_t header_addr;
  std::uint32_t load_addr;
  std::uint32_t load_end_addr;
  std::uint32_t bss_end_addr;
  std::uint32_t entry_addr;
};

/* The symbol table for a.out. */
struct AoutSymbolTable
{
  std::uint32_t tabsize;
  std::uint32_t strsize;
  std::uint32_t addr;
  std::uint32_t reserved;
} ;

/* The section header table for ELF. */
struct ElfSectionHeaderTable
{
  std::uint32_t num;
  std::uint32_t size;
  std::uint32_t addr;
  std::uint32_t shndx;
};

/* The Multiboot information. */
struct MultibootInfo
{
  std::uint32_t flags;
  std::uint32_t memLower;
  std::uint32_t mumUpper;
  std::uint32_t bootDevice;
  std::uint32_t cmdLine;
  std::uint32_t modsCount;
  std::uint32_t modsAddress;
  union
  {
    AoutSymbolTable aoutSym;
    ElfSectionHeaderTable elfSec;
  } u;
  std::uint32_t mmapLength;
  std::uint32_t mmapAddress;
};

/* The module structure. */
struct ModuleInfo
{
  std::uint32_t modStart;
  std::uint32_t modEnd;
  std::uint32_t string;
  std::uint32_t reserved;
};

/* The memory map. Be careful that the offset 0 is base_addr_low
  but no size. */
struct MemoryMap
{
  std::uint32_t size;
  uint64_t address;
  uint64_t length;
  std::uint32_t type;
};

class Multiboot {
    public:
        Multiboot(MultibootInfo& info)  : m_multibootInfo(info) {};
        void LoadModules();
        std::uint32_t LoadMemoryMap();
    private:
    void ReserveModuleMemory();
    MultibootInfo& m_multibootInfo;
};

}
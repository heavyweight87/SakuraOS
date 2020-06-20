#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Multiboot.h"

namespace MemoryManager {

#define MEMORY_NONE (0)
#define MEMORY_USER (1 << 0)
#define MEMORY_CLEAR (1 << 1)
typedef unsigned int MemoryFlags;

#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORY_ENTRY_COUNT 1024
#define MAX_NUM_PHYSICAL_PAGES (0x100000000ULL / PAGE_SIZE)

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN(size) ((size) + PAGE_SIZE - ((size) % PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x) ((__x) - ((__x) % PAGE_SIZE))

#define IS_PAGE_ALIGN(__x) (__x % PAGE_SIZE == 0)

typedef union __attribute__((packed)) PageTableEntry
{
    struct __attribute__((packed))
    {
        bool present : 1;
        bool write : 1;
        bool user : 1;
        bool pageLevelWriteThrough : 1;
        bool pageLevelCacheDisable : 1;
        bool accessed : 1;
        bool dirty : 1;
        bool pat : 1;
        uint32_t ignored : 4;
        uint32_t pageFrameNumber : 20;
    };
    uint32_t pageTableEntry;
} ;

typedef struct __attribute__((packed))
{
    PageTableEntry entries[PAGE_TABLE_ENTRY_COUNT];
} PageTable;

union __attribute__((packed)) PageDirectoryEntry
{
    struct __attribute__((packed))
    {
        bool present : 1;
        bool write : 1;
        bool user : 1;
        bool pageLevelWriteThrough : 1;
        bool pageLevelCacheDisable : 1;
        bool accessed : 1;
        bool ignored1 : 1;
        bool largePage : 1;
        uint32_t ignored : 4;
        uint32_t pageFrameNumber : 20;
    };
    uint32_t pageDirectoryEntry;
} ;

struct __attribute__((packed)) PageDirectory
{
    PageDirectoryEntry entries[PAGE_DIRECTORY_ENTRY_COUNT];
};



extern "C" void paging_enable(void);
extern "C" void paging_load_directory(uintptr_t directory);
extern "C" void paging_invalidate_tlb(void);

uintptr_t memory_alloc_identity_page(PageDirectory *pdir);

void Init(Multiboot::Multiboot& multiboot);
void memory_pdir_switch(PageDirectory *pdir);
PageDirectory *memory_kpdir(void);
uintptr_t MemoryAllocate(PageDirectory *pdir, size_t size, MemoryFlags flags);

}
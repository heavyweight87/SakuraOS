#pragma once

#include "Arch.h"

namespace MemoryManager{

#define NUM_KERNEL_PAGETABLES 256 //!< Equates to 1gb. Reserved for the kernel

#define PAGE_SIZE 0x1000
#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORY_ENTRY_COUNT 1024
#define MAX_NUM_PHYSICAL_PAGES (TOTAL_MEMORY / PAGE_SIZE)

#define PAGE_ALIGN(size) ((size) + PAGE_SIZE - ((size) % PAGE_SIZE))

union __attribute__((packed)) PageTableEntry
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
};

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

extern "C" void enablePaging(void);
extern "C" void changePageDirectory(uintptr_t directory);
extern "C" void flushCurrentPageDirectory(void);

}
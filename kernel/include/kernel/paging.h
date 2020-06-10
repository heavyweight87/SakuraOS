#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_DIRECTORY_ENTRY_COUNT 1024

#define PAGE_SIZE 0x1000
#define PAGE_ALIGN(__x) ((__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))

#define PAGE_ALIGN_UP(__x)  \
    ((__x % PAGE_SIZE == 0) \
         ? (__x)            \
         : (__x) + PAGE_SIZE - ((__x) % PAGE_SIZE))

#define PAGE_ALIGN_DOWN(__x) ((__x) - ((__x) % PAGE_SIZE))

#define IS_PAGE_ALIGN(__x) (__x % PAGE_SIZE == 0)

typedef union __attribute__((packed)) // page table entry
{
    struct __attribute__((packed))
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Dirty : 1;
        bool Pat : 1;
        uint32_t Ignored : 4;
        uint32_t PageFrameNumber : 20;
    };

    uint32_t as_uint32;
} PageTableEntry;

typedef struct __attribute__((packed))
{
    PageTableEntry entries[PAGE_TABLE_ENTRY_COUNT];
} PageTable;

typedef union __attribute__((packed)) {
    struct __attribute__((packed))
    {
        bool Present : 1;
        bool Write : 1;
        bool User : 1;
        bool PageLevelWriteThrough : 1;
        bool PageLevelCacheDisable : 1;
        bool Accessed : 1;
        bool Ignored1 : 1;
        bool LargePage : 1;
        uint32_t Ignored2 : 4;
        uint32_t PageFrameNumber : 20;
    };
    uint32_t as_uint32;
} PageDirectoryEntry;

typedef struct __attribute__((packed))
{
    PageDirectoryEntry entries[PAGE_DIRECTORY_ENTRY_COUNT];
} PageDirectory;

extern void paging_enable(void);
extern void paging_disable(void);
extern void paging_load_directory(uintptr_t directory);
extern void paging_invalidate_tlb(void);

void paging_init();
void memory_pdir_switch(PageDirectory *pdir);
PageDirectory *memory_kpdir(void);
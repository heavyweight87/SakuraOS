#include <stdio.h>
#include <stddef.h>
#include "kernel/multiboot.h"
#include "kernel/paging.h"
#include "kernel/physical.h"
#include "kernel/virtual.h"



extern uint32_t kernel_end;
extern int __start;

static inline MemoryRange memory_range_around_non_aligned_address(uintptr_t base, size_t size)
{
    size_t align = base % PAGE_SIZE;

    base -= align;
    size += align;

    size += PAGE_SIZE - size % PAGE_SIZE;

    return (MemoryRange){base, size};
}

static MemoryRange kernel_memory_range(void)
{
    return memory_range_around_non_aligned_address((uintptr_t)&__start, (size_t)&kernel_end - (size_t)&__start);
}

void paging_init()
{
    printf("Initializing memory management...");

    for (size_t i = 0; i < 1024 * 1024 / 8; i++)
    {
        MEMORY[i] = 0xff;
    }

    // Setup the kernel pagedirectory.
    for (int i = 0; i < 256; i++)
    {
        PageDirectoryEntry *entry = &kpdir.entries[i];
        entry->User = 0;
        entry->Write = 1;
        entry->Present = 1;
        entry->PageFrameNumber = (uint32_t)&kptable[i] / PAGE_SIZE;
    }

    physical_set_free(kernel_end, 0x100000000 / PAGE_SIZE);

    USED_MEMORY = 0;

    printf("Mapping kernel...");
    memory_map_eternal(&kpdir, kernel_memory_range());

    virtual_unmap(memory_kpdir(), 0, 1); // Unmap the 0 page
    physical_set_used(0, 1);

    memory_pdir_switch(&kpdir);
    paging_enable();

    printf("%uKio of memory detected", TOTAL_MEMORY / 1024);
    printf("%uKio of memory is used by the kernel", USED_MEMORY / 1024);

    printf("Paging enabled!");
}

void memory_pdir_switch(PageDirectory *pdir)
{
    paging_load_directory(virtual2physical(&kpdir, (uintptr_t)pdir));
}

PageDirectory *memory_kpdir(void)
{
    return &kpdir;
}
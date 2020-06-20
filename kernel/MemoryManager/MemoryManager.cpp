#include <stdio.h>
#include <stddef.h>
#include "Paging.h"
#include "PhysicalAllocator.h"
#include "VirtualMemoryManager.h"
#include <string.h>


namespace MemoryManager {

//these can be found in the linker
extern "C" uint32_t __start;
extern "C" uint32_t _kernel_end;

void MapKernelMemory()
{
    
    uintptr_t kernelStart = (uintptr_t)&__start;
    size_t kernelSize = (size_t)&_kernel_end - (size_t)&__start;
    kernelStart -= (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += PAGE_SIZE - (kernelSize % PAGE_SIZE);

    memory_map_eternal(&kpdir, kernelStart, kernelSize);
}


void InitPaging(Multiboot::Multiboot& multiboot)
{
    // Setup the kernel pagedirectory.
    for (int i = 0; i < 256; i++)
    {
        PageDirectoryEntry *entry = &kpdir.entries[i];
        entry->User = 0;
        entry->Write = 1;
        entry->Present = 1;
        entry->PageFrameNumber = (uint32_t)&kptable[i] / PAGE_SIZE;
    }
    InitPhysicalAllocator(multiboot);
    MapKernelMemory();
  //  MapKernelMemory();
    virtual_unmap(memory_kpdir(), 0, 1); // Unmap the 0 page
    physical_set_used(0, 1);

    memory_pdir_switch(&kpdir);
    paging_enable();

   // printf("%uKio of memory detected", TOTAL_MEMORY / 1024);
    //printf("%uKio of memory is used by the kernel", USED_MEMORY / 1024);
}

void memory_pdir_switch(PageDirectory *pdir)
{
    paging_load_directory(virtual2physical(&kpdir, (uintptr_t)pdir));
}

PageDirectory *memory_kpdir(void)
{
    return &kpdir;
}

uintptr_t memory_alloc_identity_page(PageDirectory *pdir)
{
   // atomic_begin();

    for (size_t i = 1; i < 256 * 1024; i++)
    {
        int address = i * PAGE_SIZE;

        if (!page_present(pdir, address) &&
            !physical_is_used(address, 1))
        {
            physical_set_used(address, 1);
            virtual_map(pdir, address, address, 1, false);

          //  atomic_end();

            memset((void *)address, 0, PAGE_SIZE);

            return address;
        }
    }

   // atomic_end();

    printf("Failled to allocate identity mapped page!");
    return 0;
}

uintptr_t memory_alloc(PageDirectory *page_directory, size_t size, MemoryFlags flags)
{
    if (!size)
        return 0;

    size_t page_count = PAGE_ALIGN_UP(size) / PAGE_SIZE;

  ///  atomic_begin();

    uintptr_t physical_address = physical_alloc(page_count);

    if (!physical_address)
    {
       // atomic_end();

        printf("Failled to allocate memory: not enough physical memory!");

        return 0;
    }

    uintptr_t virtual_address = virtual_alloc(
        page_directory,
        physical_address,
        page_count,
        flags & MEMORY_USER);

    if (!virtual_address)
    {
        FreePhysical(physical_address, page_count);
    //    atomic_end();

        printf("Failled to allocate memory: not enough virtual memory!");

        return 0;
    }

   // atomic_end();

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_address, 0, page_count * PAGE_SIZE);

    return virtual_address;
}

PageDirectory *memory_pdir_create() //for user
{
   // atomic_begin();

    PageDirectory *pdir = (PageDirectory *)memory_alloc(&kpdir, sizeof(PageDirectory), MEMORY_CLEAR);

    if (pdir == NULL)
    {
        printf("Page directory allocation failled!");
        return NULL;
    }

    memset(pdir, 0, sizeof(PageDirectory));

    // Copy first gigs of virtual memory (kernel space);
    for (int i = 0; i < 256; i++)
    {
        PageDirectoryEntry *e = &pdir->entries[i];
        e->User = 0;
        e->Write = 1;
        e->Present = 1;
        e->PageFrameNumber = (uint32_t)&kptable[i] / PAGE_SIZE;
    }

   // atomic_end();

    return pdir;
}

}

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

#define NUM_KERNEL_PAGETABLES 256 //!< Equates to 1gb. Reserved for the kernel

PageDirectory kernelPageDirectory __attribute__ ((aligned(PAGE_SIZE))) = {};
PageTable kernelPageTable[NUM_KERNEL_PAGETABLES] __attribute__ ((aligned(PAGE_SIZE))) = {};

static void MapKernelPageTable(PageDirectory& pageDirectory)
{
    for (int index = 0; index < NUM_KERNEL_PAGETABLES; index++)
    {
        PageDirectoryEntry *entry = &pageDirectory.entries[index];
        entry->User = 0;
        entry->Write = 1;
        entry->Present = 1;
        entry->PageFrameNumber = (uint32_t)&kernelPageTable[index] / PAGE_SIZE;
    }
}

static void MapKernelMemory()
{
    uintptr_t kernelStart = (uintptr_t)&__start;
    size_t kernelSize = (size_t)&_kernel_end - (size_t)&__start;
    kernelStart -= (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += PAGE_SIZE - (kernelSize % PAGE_SIZE);
    MapKernelPageTable(kernelPageDirectory); //map the kernel page directory
    IdentityMap(&kernelPageDirectory, kernelStart, kernelSize);
    //the first page directory entry is reserved for bootloader and other things
    VirtualFree(&kernelPageDirectory, 0, 1); // free first 1mb
    PhysicalAllocate(0, 1); 
}

void Init(Multiboot::Multiboot& multiboot)
{
    PhysicalAllocatorInit(multiboot);
    MapKernelMemory();
    memory_pdir_switch(&kernelPageDirectory);
    paging_enable();
}

void memory_pdir_switch(PageDirectory *pdir)
{
    paging_load_directory(virtual2physical(&kernelPageDirectory, (uintptr_t)pdir));
}

PageDirectory *memory_kpdir(void)
{
    return &kernelPageDirectory;
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
            PhysicalAllocate(address, 1);
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

uintptr_t MemoryAllocate(PageDirectory *page_directory, size_t size, MemoryFlags flags)
{
    std::uint32_t numPages = PAGE_ALIGN(size) / PAGE_SIZE;

  ///  atomic_begin();

    uintptr_t physical_address = PhysicalAllocate(numPages);

    if (!physical_address)
    {
       // atomic_end();

        printf("Failled to allocate memory: not enough physical memory!");

        return 0;
    }

    uintptr_t virtual_address = virtual_alloc(
        page_directory,
        physical_address,
        numPages,
        flags & MEMORY_USER);

    if (!virtual_address)
    {
        PhysicalFree(physical_address, numPages);
    //    atomic_end();

        printf("Failled to allocate memory: not enough virtual memory!");

        return 0;
    }

   // atomic_end();

    if (flags & MEMORY_CLEAR)
        memset((void *)virtual_address, 0, numPages * PAGE_SIZE);

    return virtual_address;
}

PageDirectory *CreateUserPageDirectory() //for user
{
   // atomic_begin();

    PageDirectory *pageDirectory = (PageDirectory *)MemoryAllocate(&kernelPageDirectory, sizeof(PageDirectory), MEMORY_CLEAR);

    if (pageDirectory == NULL)
    {
        printf("Page directory allocation failled!");
        return NULL;
    }

    memset(pageDirectory, 0, sizeof(PageDirectory));

    MapKernelPageTable(*pageDirectory);


   // atomic_end();

    return pageDirectory;
}

}

#include <stdio.h>
#include <stddef.h>
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include "VirtualMemoryManager.h"
#include <string.h>


namespace MemoryManager {

//these can be found in the linker
extern "C" uint32_t __start;
extern "C" uint32_t _kernel_end;

PageDirectory kernelPageDirectory __attribute__ ((aligned(PAGE_SIZE)));
PageTable kernelPageTable[NUM_KERNEL_PAGETABLES] __attribute__ ((aligned(PAGE_SIZE)));

static void MapKernelPageTable(PageDirectory& pageDirectory)
{
    for (int index = 0; index < NUM_KERNEL_PAGETABLES; index++)
    {
        PageDirectoryEntry *directoryEntry = &pageDirectory.entries[index];
        directoryEntry->user = 0;
        directoryEntry->write = 1;
        directoryEntry->present = 1;
        directoryEntry->pageFrameNumber = (uint32_t)&kernelPageTable[index] / PAGE_SIZE; //we only need the page number
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
  //  VirtualFree(kernelPageDirectory, 0, 1); // free first 4mb
    //PhysicalAllocate(0, 1); 
}

void Init(Multiboot::Multiboot& multiboot)
{
    PhysicalAllocatorInit(multiboot);
    MapKernelMemory();
    IdentityMap(&kernelPageDirectory, 0, (size_t)&__start); //its much easier to indentity map first 1mb for hardware bound addresses
    memory_pdir_switch(&kernelPageDirectory);
    paging_enable();
    //IdentityMap(&kernelPageDirectory, 4096, 0x100000 - 4096);
}

void memory_pdir_switch(PageDirectory *pdir)
{
    paging_load_directory(virtual2physical(&kernelPageDirectory, (uintptr_t)pdir));
}

PageDirectory& memory_kpdir(void)
{
    return kernelPageDirectory;
}

uintptr_t AllocatePageTable(PageDirectory *pdir)
{
   // atomic_begin();

    for (size_t i = 1; i < 256 * 1024; i++)
    {
        int address = i * PAGE_SIZE;

        if (!IsPagePresent(*pdir, address) &&
            !physical_is_used(address, 1))
        {
            PhysicalAllocate(address, 1);
            VirtualMap(*pdir, address, address, 1, false);

          //  atomic_end();

            memset((void *)address, 0, PAGE_SIZE);

            return address;
        }
    }

   // atomic_end();

    printf("Failled to allocate identity mapped page!");
    return 0;
}

uintptr_t MemoryAllocate(PageDirectory& pageDirectory, size_t size, bool user)
{
    std::uint32_t numPages = PAGE_ALIGN(size) / PAGE_SIZE;

  ///  atomic_begin();
    //firstly allocate physical memory
    uintptr_t physicalAddress = PhysicalAllocate(numPages);

    if (physicalAddress > 0)
    {
        uintptr_t virtual_address = VirtualAllocate(pageDirectory, physicalAddress, numPages, user);
        if (!virtual_address)
        {
            PhysicalFree(physicalAddress, numPages);
        //    atomic_end();

            printf("Failled to allocate memory: not enough virtual memory!");

            return 0;
        }

    // atomic_end();

        memset((void *)virtual_address, 0, numPages * PAGE_SIZE);
        return virtual_address;
    }
    return 0; 

}

PageDirectory *CreateUserPageDirectory() //for user
{
   // atomic_begin();

    PageDirectory *pageDirectory = (PageDirectory *)MemoryAllocate(kernelPageDirectory, sizeof(PageDirectory), false);

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

#include <stdio.h>
#include <stddef.h>
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include "VirtualMemoryManager.h"
#include <string.h>
#include "Scheduler.h"


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
    IdentityMap(kernelPageDirectory, kernelStart, kernelSize);
}

void Init(Multiboot::Multiboot& multiboot)
{
    PhysicalAllocatorInit(multiboot);
    MapKernelMemory();
    //its much easier to indentity map first 1mb for hardware bound addresses
    IdentityMap(kernelPageDirectory, 0, (size_t)&__start);
    ChangePageDirectory(GetPhysicalAddress(kernelPageDirectory, (uintptr_t)&kernelPageDirectory));
    EnablePaging();
}

uintptr_t MemoryAllocate(size_t size, bool user)
{
    PageDirectory *pageDirectory = (PageDirectory*)Scheduler::GetRunningTask().regs.cr3;
    return MemoryAllocate(*pageDirectory, size, user);
}

uintptr_t MemoryAllocate(PageDirectory& pageDirectory, size_t size, bool user)
{
    std::uint32_t numPages = size / PAGE_SIZE;
    if(size % PAGE_SIZE)
    {
        numPages = PAGE_ALIGN(size) / PAGE_SIZE;
    }

    //  atomic_begin();
    //firstly allocate physical memory
    uintptr_t physicalAddress = PhysicalAllocate(numPages);

    if (physicalAddress > 0)
    {
        uintptr_t virtualAddress = VirtualAllocate(pageDirectory ,physicalAddress, numPages, user);
        if (virtualAddress == 0)
        {
            PhysicalFree(physicalAddress, numPages);
        //    atomic_end();

            printf("Out of vmem!\r\n");
            return 0;
        }

    // atomic_end();

        memset((void *)virtualAddress, 0, numPages * PAGE_SIZE); //clear the memory
        return virtualAddress;
    }
    return 0; 
}

void MemoryMap(PageDirectory& pageDirectory, uint32_t virtualAddress, size_t size, bool isUser)
{
    std::uint32_t numPages = size / PAGE_SIZE;
    if(size % PAGE_SIZE)
    {
        numPages = PAGE_ALIGN(size) / PAGE_SIZE;
    }
    std::uint32_t physicalAddress = PhysicalAllocate(numPages);
    VirtualMap(pageDirectory, virtualAddress, physicalAddress, numPages, isUser);
}

void MemoryFree(PageDirectory& pageDirectory, uint32_t startAddress, size_t size)
{
   // atomic_begin();
    uint32_t numPages = 0;
    for (uint32_t i = 0; i < size/ PAGE_SIZE; i++)
    {
        uint32_t virtual_address = startAddress + i * PAGE_SIZE;

        if (IsPagePresent(pageDirectory, virtual_address))
        {
            PhysicalFree(GetPhysicalAddress(pageDirectory, virtual_address), 1);
            VirtualFree(pageDirectory, virtual_address, 1);
            numPages++;
        }
    }
 //   atomic_end();
}

PageDirectory *CreateUserPageDirectory() 
{
   // atomic_begin();

    PageDirectory *pageDirectory = (PageDirectory *)MemoryAllocate(sizeof(PageDirectory), false);
    if (pageDirectory == NULL)
    {
        printf("Could not allocate page directory...\r\n");
        return NULL;
    }
    MapKernelPageTable(*pageDirectory);

   // atomic_end();

    return pageDirectory;
}

void IdentityMap(PageDirectory& pageDirectory, uintptr_t address, size_t size)
{
    size_t page_count = PAGE_ALIGN(size) / PAGE_SIZE;

    //atomic_begin();
    PhysicalAllocate(address, page_count);
    VirtualMap(pageDirectory, address, address, page_count, false);
   // atomic_end();
}

PageDirectory& GetKerkelPageDirectory()
{
    return kernelPageDirectory;
}

}

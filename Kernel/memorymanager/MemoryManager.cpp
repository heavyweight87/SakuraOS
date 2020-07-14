#include "Libk.h"
#include <stddef.h>
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include "VirtualMemoryManager.h"
#include "Scheduler.h"
#include "Atomic.h"


namespace MemoryManager {

//these can be found in the linker
extern "C" uint32_t __start;
extern "C" uint32_t _kernel_end;

PageDirectory kernelPageDirectory __attribute__ ((aligned(PAGE_SIZE)));
PageTable kernelPageTable[NUM_KERNEL_PAGETABLES] __attribute__ ((aligned(PAGE_SIZE)));

static void mapKernelPageTable(PageDirectory& pageDirectory)
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

static void mapKernelMemory()
{
    uintptr_t kernelStart = (uintptr_t)&__start;
    size_t kernelSize = (size_t)&_kernel_end - (size_t)&__start;
    kernelStart -= (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += (uintptr_t)&__start % PAGE_SIZE;
    kernelSize += PAGE_SIZE - (kernelSize % PAGE_SIZE);
    mapKernelPageTable(kernelPageDirectory); //map the kernel page directory
    identityMap(kernelPageDirectory, kernelStart, kernelSize);
}

void init(Multiboot::Multiboot& multiboot)
{
    physicalAllocatorInit(multiboot);
    mapKernelMemory();
    //its much easier to indentity map first 1mb for hardware bound addresses
    identityMap(kernelPageDirectory, 0, (size_t)&__start);
    multiboot.reserveModuleMemory();
    changePageDirectory(GetPhysicalAddress(kernelPageDirectory, (uintptr_t)&kernelPageDirectory));
    enablePaging();
}

uintptr_t memoryAllocate(size_t size, bool user)
{
    PageDirectory *pageDirectory = (PageDirectory*)Scheduler::getRunningTask().regs.cr3;
    return memoryAllocate(*pageDirectory, size, user);
}

uintptr_t memoryAllocate(PageDirectory& pageDirectory, size_t size, bool user)
{
    std::uint32_t numPages = size / PAGE_SIZE;
    if(size % PAGE_SIZE)
    {
        numPages = PAGE_ALIGN(size) / PAGE_SIZE;
    }

    Libk::atomicEnable();
    //firstly allocate physical memory
    uintptr_t physicalAddress = physicalAllocate(numPages);

    if (physicalAddress > 0)
    {
        uintptr_t virtualAddress = VirtualAllocate(pageDirectory ,physicalAddress, numPages, user);
        if (virtualAddress == 0)
        {
            physicalFree(physicalAddress, numPages);
            Libk::atomicEnable();
            Libk::printk("Out of vmem!\r\n");
            return 0;
        }

        Libk::atomicEnable();
        Libk::memset((void *)virtualAddress, 0, numPages * PAGE_SIZE); //clear the memory
        return virtualAddress;
    }
    return 0; 
}

void memoryMap(PageDirectory& pageDirectory, uint32_t virtualAddress, size_t size, bool isUser)
{
    std::uint32_t numPages = size / PAGE_SIZE;
    if(size % PAGE_SIZE)
    {
        numPages = PAGE_ALIGN(size) / PAGE_SIZE;
    }
    std::uint32_t physicalAddress = physicalAllocate(numPages);
    VirtualMap(pageDirectory, virtualAddress, physicalAddress, numPages, isUser);
}

void memoryFree(int32_t startAddress, size_t size)
{
    PageDirectory *pageDirectory = (PageDirectory*)Scheduler::getRunningTask().regs.cr3;
    memoryFree(*pageDirectory, startAddress, size);
}

void memoryFree(PageDirectory& pageDirectory, uint32_t startAddress, size_t size)
{
    Libk::atomicEnable();
    uint32_t numPages = 0;
    for (uint32_t i = 0; i < size/ PAGE_SIZE; i++)
    {
        uint32_t virtual_address = startAddress + i * PAGE_SIZE;

        if (IsPagePresent(pageDirectory, virtual_address))
        {
            physicalFree(GetPhysicalAddress(pageDirectory, virtual_address), 1);
            VirtualFree(pageDirectory, virtual_address, 1);
            numPages++;
        }
    }
    Libk::atomicDisable();
}

PageDirectory *createUserPageDirectory() 
{
   Libk::atomicEnable();

    PageDirectory *pageDirectory = (PageDirectory *)memoryAllocate(sizeof(PageDirectory), false);
    if (pageDirectory == NULL)
    {
        Libk::printk("Could not allocate page directory...\r\n");
        Libk::atomicDisable();
        return NULL;
    }
    mapKernelPageTable(*pageDirectory);

   Libk::atomicDisable();

    return pageDirectory;
}

void identityMap(PageDirectory& pageDirectory, uintptr_t address, size_t size)
{
    size_t page_count = PAGE_ALIGN(size) / PAGE_SIZE;

    Libk::atomicEnable();
    physicalAllocate(address, page_count);
    VirtualMap(pageDirectory, address, address, page_count, false);
    Libk::atomicDisable();
}

PageDirectory& getKerkelPageDirectory()
{
    return kernelPageDirectory;
}

}


#include "VirtualMemoryManager.h"
#include "MemoryManager.h"
#include "PhysicalAllocator.h"
#include "Libk.h"
#include <stddef.h>

namespace MemoryManager {

/**
 * A virtual address is made up of the following:
 * 12 bits for the offset within the page (4096 bytes)
 * 10 bits for the page directory offset
 * 10 bits for the page table offset
 */

#define PAGE_DIR_INDEX(vaddress) ((vaddress) >> 22)
#define PAGE_TABLE_INDEX(vaddress) (((vaddress) >> 12) & 0x03ff)

static PageDirectoryEntry& GetPageDirectoryEntry(PageDirectory& pageDirectory, uint32_t virtualAddress)
{
    uint32_t directoryIndex = PAGE_DIR_INDEX(virtualAddress);
    return pageDirectory.entries[directoryIndex];
}

static PageTable& GetPageTable(PageDirectory& pageDirectory, uint32_t virtualAddress)
{
    return *(PageTable*)(GetPageDirectoryEntry(pageDirectory, virtualAddress).pageFrameNumber * PAGE_SIZE);
}

static PageTableEntry& GetPageTableEntry(PageDirectory& pageDirectory, uint32_t virtualAddress)
{
    uint32_t pageTableIndex = PAGE_TABLE_INDEX(virtualAddress);
    return GetPageTable(pageDirectory, virtualAddress).entries[pageTableIndex];
}

void VirtualMap(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t numPages, bool isUser)
{
    for (uint32_t pageIndex = 0; pageIndex < numPages; pageIndex++)
    {
        PageDirectoryEntry& pageDirectoryEntry = GetPageDirectoryEntry(pageDirectory, virtualAddress);

        if (pageDirectoryEntry.present == false)
        {
            uint32_t pageTablePhysicalAddress = PhysicalAllocate(1);
            IdentityMap(pageDirectory, pageTablePhysicalAddress, false);
            PageTable *pageTable = (PageTable*)pageTablePhysicalAddress;
            pageDirectoryEntry.present = 1;
            pageDirectoryEntry.write = 1;
            pageDirectoryEntry.user = isUser;
            pageDirectoryEntry.pageFrameNumber = (uint32_t)(pageTable) >> 12;
        }

        PageTableEntry& pageTableEntry = GetPageTableEntry(pageDirectory, virtualAddress);
        pageTableEntry.present = 1;
        pageTableEntry.write = 1;
        pageTableEntry.user = isUser;
        pageTableEntry.pageFrameNumber = physicalAddress >> 12;

        virtualAddress += PAGE_SIZE;
        physicalAddress += PAGE_SIZE;
    }

    FlushCurrentPageDirectory();
}

bool IsPagePresent(PageDirectory& pageDirectory, uint32_t virtualAddress)
{
    PageDirectoryEntry directoryEntry = GetPageDirectoryEntry(pageDirectory, virtualAddress);
    if (directoryEntry.present)
    {
        return GetPageTableEntry(pageDirectory, virtualAddress).present;
    }
    return false;
}

uint32_t GetPhysicalAddress(PageDirectory& pageDirectory, uint32_t virtualAddress)
{
    PageTableEntry& pageTableEntry = GetPageTableEntry(pageDirectory, virtualAddress);
    return (pageTableEntry.pageFrameNumber * PAGE_SIZE) + (virtualAddress&0xFFF);
}

void VirtualFree(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t numPages)
{
    for (uint32_t address = virtualAddress; address < virtualAddress + (numPages * PAGE_SIZE); address += PAGE_SIZE)
    {
        GetPageTableEntry(pageDirectory, address).pageTableEntry = 0;
    }
    FlushCurrentPageDirectory();
}

uint32_t VirtualAllocate(PageDirectory& pageDirectory, uint32_t physicalAddress, uint32_t numPages, bool user)
{
    uint32_t startAddr = 0;
    uint32_t userStartAddress = NUM_KERNEL_PAGETABLES * PAGE_SIZE * 1024;
    uint32_t virtualAddress = KERNEL_START_ADDRESS; 
    uint32_t maxAddress =  userStartAddress;
    uint32_t pageIndex = 0;
    if(user) //user memory starts after kernel TODO: higher half kernel
    {
       virtualAddress =  userStartAddress; //after kernel memory
       maxAddress = TOTAL_MEMORY;
    }
    startAddr = virtualAddress;

    // try to find a range of free virtual address - this is horribly inefficient TODO: improve
    while(virtualAddress < maxAddress)
    {
        //we need the virtual address to be in a continious sequence
        if (!IsPagePresent(pageDirectory, virtualAddress)) //this page is free
        {
            pageIndex++;
            if (pageIndex == numPages)
            {
                VirtualMap(pageDirectory, startAddr, physicalAddress, numPages, user);
                return startAddr;
            }
        }
        else //page is taken, reset
        {
            pageIndex = 0;
            startAddr = virtualAddress + PAGE_SIZE;
        }
        virtualAddress += PAGE_SIZE;
    }

    Libk::printk("Out of virtual memory!");
    return 0;
}
}

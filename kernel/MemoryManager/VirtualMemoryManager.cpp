
#include "VirtualMemoryManager.h"
#include "Paging.h"
#include <PhysicalAllocator.h>
#include <stdio.h>
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

bool IsPagePresent(PageDirectory &pageDirectory, uint32_t virtualAddress)
{
    PageDirectoryEntry *directoryEntry = &pageDirectory.entries[PAGE_DIR_INDEX(virtualAddress)];

    if (directoryEntry->present)
    {
        PageTable *pageTable = (PageTable *)(directoryEntry->pageFrameNumber * PAGE_SIZE);
        PageTableEntry *pageTableEntry = &pageTable->entries[PAGE_TABLE_INDEX(virtualAddress)];
        return pageTableEntry->present;
    }
    return false;
}

int virtual_present(PageDirectory *pdir, uint32_t vaddr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t offset = i * PAGE_SIZE;

        if (!IsPagePresent(*pdir, vaddr + offset))
        {
            return 0;
        }
    }

    return 1;
}

uint32_t virtual2physical(PageDirectory *pdir, uint32_t vaddr)
{
    uint32_t pdi = PAGE_DIR_INDEX(vaddr);
    uint32_t pti = PAGE_TABLE_INDEX(vaddr);

    PageDirectoryEntry *pde = &pdir->entries[pdi];
    PageTable *ptable = (PageTable *)(pde->pageFrameNumber * PAGE_SIZE);
    PageTableEntry *p = &ptable->entries[pti];
    uint32_t phys = (p->pageFrameNumber * PAGE_SIZE) + (vaddr & 0xfff);
    return phys;
}

void VirtualMap(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t numPages, bool isUser)
{
    for (uint32_t index = 0; index < numPages; index++)
    {
        uint32_t offset = index * PAGE_SIZE;

        uint32_t pdi = PAGE_DIR_INDEX(virtualAddress + offset);
        uint32_t pti = PAGE_TABLE_INDEX(virtualAddress + offset);

        PageDirectoryEntry *pde = &pageDirectory.entries[pdi];
        PageTable *ptable = (PageTable *)(pde->pageFrameNumber * PAGE_SIZE);

        if (!pde->present)
        {
            ptable = (PageTable *)memory_alloc_identity_page(&pageDirectory);

            pde->present = 1;
            pde->write = 1;
            pde->user = isUser;
            pde->pageFrameNumber = (uint32_t)(ptable) >> 12;
        }

        PageTableEntry *p = &ptable->entries[pti];
        p->present = 1;
        p->write = 1;
        p->user = isUser;
        p->pageFrameNumber = (physicalAddress + offset) >> 12;
    }

    paging_invalidate_tlb();
}

void VirtualFree(PageDirectory *pdir, uint32_t virtualAddress, uint32_t numPages)
{
    for (uint32_t i = 0; i < numPages; i++)
    {
        uint32_t offset = i * PAGE_SIZE;

        uint32_t pdi = PAGE_DIR_INDEX(virtualAddress + offset);
        uint32_t pti = PAGE_TABLE_INDEX(virtualAddress + offset);

        PageDirectoryEntry *pde = &pdir->entries[pdi];
        PageTable *ptable = (PageTable *)(pde->pageFrameNumber * PAGE_SIZE);
        PageTableEntry *p = &ptable->entries[pti];

        if (pde->present)
            p->pageFrameNumber = 0;
    }

    paging_invalidate_tlb();
}

uint32_t virtual_alloc(PageDirectory *pdir, uint32_t paddr, uint32_t count, int user)
{
    if (count == 0)
        return 0;

    uint32_t current_size = 0;
    uint32_t startaddr = 0;

    // we skip the first page to make null deref trigger a page fault
    for (size_t i = (user ? 256 : 1) * 1024; i < (user ? 1024 : 256) * 1024; i++)
    {
        int vaddr = i * PAGE_SIZE;

        if (!IsPagePresent(*pdir, vaddr))
        {
            if (current_size == 0)
            {
                startaddr = vaddr;
            }

            current_size++;

            if (current_size == count)
            {
                VirtualMap(*pdir, startaddr, paddr, count, user);
                return startaddr;
            }
        }
        else
        {
            current_size = 0;
        }
    }

    printf("Out of virtual memory!");
    return 0;
}

int IdentityMap(PageDirectory *pdir, uintptr_t address, size_t size)
{
    size_t page_count = PAGE_ALIGN(size) / PAGE_SIZE;

    //atomic_begin();
    PhysicalAllocate(address, page_count);
    VirtualMap(*pdir, address, address, page_count, false);
   // atomic_end();
   
    return 0;
}
}

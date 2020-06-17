
#include <virtual.h>
#include <paging.h>
#include <physical.h>
#include <stdio.h>
#include <stddef.h>

#define PD_INDEX(vaddr) ((vaddr) >> 22)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x03ff)

PageDirectory kpdir __attribute__ ((aligned(PAGE_SIZE))) = {};
PageTable kptable[256] __attribute__ ((aligned(PAGE_SIZE))) = {};

int page_present(PageDirectory *pdir, uint32_t vaddr)
{
    uint32_t pdi = PD_INDEX(vaddr);
    uint32_t pti = PT_INDEX(vaddr);

    PageDirectoryEntry *pde = &pdir->entries[pdi];

    if (!pde->Present)
    {
        return 0;
    }

    PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *p = &ptable->entries[pti];

    if (!p->Present)
    {
        return 0;
    }

    return 1;
}

int virtual_present(PageDirectory *pdir, uint32_t vaddr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t offset = i * PAGE_SIZE;

        if (!page_present(pdir, vaddr + offset))
        {
            return 0;
        }
    }

    return 1;
}

uint32_t virtual2physical(PageDirectory *pdir, uint32_t vaddr)
{
    uint32_t pdi = PD_INDEX(vaddr);
    uint32_t pti = PT_INDEX(vaddr);

    PageDirectoryEntry *pde = &pdir->entries[pdi];
    PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
    PageTableEntry *p = &ptable->entries[pti];
    uint32_t phys = (p->PageFrameNumber * PAGE_SIZE) + (vaddr & 0xfff);
    return phys;
}

int virtual_map(PageDirectory *pdir, uint32_t vaddr, uint32_t paddr, uint32_t count, bool user)
{
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t offset = i * PAGE_SIZE;

        uint32_t pdi = PD_INDEX(vaddr + offset);
        uint32_t pti = PT_INDEX(vaddr + offset);

        PageDirectoryEntry *pde = &pdir->entries[pdi];
        PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);

        if (!pde->Present)
        {
            ptable = (PageTable *)memory_alloc_identity_page(pdir);

            pde->Present = 1;
            pde->Write = 1;
            pde->User = user;
            pde->PageFrameNumber = (uint32_t)(ptable) >> 12;
        }

        PageTableEntry *p = &ptable->entries[pti];
        p->Present = 1;
        p->Write = 1;
        p->User = user;
        p->PageFrameNumber = (paddr + offset) >> 12;
    }

    paging_invalidate_tlb();

    return 0;
}

void virtual_unmap(PageDirectory *pdir, uint32_t vaddr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t offset = i * PAGE_SIZE;

        uint32_t pdi = PD_INDEX(vaddr + offset);
        uint32_t pti = PT_INDEX(vaddr + offset);

        PageDirectoryEntry *pde = &pdir->entries[pdi];
        PageTable *ptable = (PageTable *)(pde->PageFrameNumber * PAGE_SIZE);
        PageTableEntry *p = &ptable->entries[pti];

        if (pde->Present)
            p->as_uint32 = 0;
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

        if (!page_present(pdir, vaddr))
        {
            if (current_size == 0)
            {
                startaddr = vaddr;
            }

            current_size++;

            if (current_size == count)
            {
                virtual_map(pdir, startaddr, paddr, count, user);
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

void virtual_free(PageDirectory *pdir, uint32_t vaddr, uint32_t count)
{
    // TODO: Check if the memory was allocated with ´virtual_alloc´.
    virtual_unmap(pdir, vaddr, count);
}

int memory_map_eternal(PageDirectory *pdir, MemoryRange range)
{
    size_t page_count = PAGE_ALIGN_UP(range.size) / PAGE_SIZE;

    //atomic_begin();
    physical_set_used(range.base, page_count);
    virtual_map(pdir, range.base, range.base, page_count, 0);
   // atomic_end();

    return 0;
}

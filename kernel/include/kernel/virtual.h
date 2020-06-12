#pragma once

#include "kernel/paging.h"

typedef struct
{
    uintptr_t base;
    size_t size;
} MemoryRange;

extern PageDirectory kpdir;
extern PageTable kptable[256];

int page_present(PageDirectory *pdir, uint32_t vaddr);

int virtual_present(PageDirectory *pdir, uint32_t vaddr, uint32_t count);

uint32_t virtual2physical(PageDirectory *pdir, uint32_t vaddr);

int virtual_map(PageDirectory *pdir, uint32_t vaddr, uint32_t paddr, uint32_t count, bool user);

void virtual_unmap(PageDirectory *pdir, uint32_t vaddr, uint32_t count);


#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t virtual_alloc(PageDirectory *pdir, uint32_t paddr, uint32_t count, int user);

#ifdef __cplusplus
}
#endif

uint32_t virtual_alloc(PageDirectory *pdir, uint32_t paddr, uint32_t count, int user);

void virtual_free(PageDirectory *pdir, uint32_t vaddr, uint32_t count);

int memory_map_eternal(PageDirectory *pdir, MemoryRange range);
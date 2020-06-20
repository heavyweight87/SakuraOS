#pragma once

#include "Paging.h"

namespace MemoryManager {


bool IsPagePresent(PageDirectory& pageDirectory, uint32_t virtualAddress);

int virtual_present(PageDirectory *pdir, uint32_t vaddr, uint32_t count);

uint32_t virtual2physical(PageDirectory *pdir, uint32_t vaddr);

void VirtualMap(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t numPages, bool isUser);



#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t virtual_alloc(PageDirectory *pdir, uint32_t paddr, uint32_t count, int user);

#ifdef __cplusplus
}
#endif

uint32_t virtual_alloc(PageDirectory *pdir, uint32_t paddr, uint32_t count, int user);

void VirtualFree(PageDirectory *pdir, uint32_t vaddr, uint32_t count);

int IdentityMap(PageDirectory *pdir, uintptr_t address, uint32_t size);

}
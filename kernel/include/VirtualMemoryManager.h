#pragma once

#include "MemoryManager.h"

namespace MemoryManager {


bool IsPagePresent(PageDirectory& pageDirectory, uint32_t virtualAddress);

uint32_t GetPhysicalAddress(PageDirectory& pageDirectory, uint32_t virtualAddress);

void VirtualMap(PageDirectory& pageDirectory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t numPages, bool isUser);

uint32_t VirtualAllocate(PageDirectory& pageDirectory, uint32_t physicalAddress, uint32_t numPages, bool user);

void VirtualFree(PageDirectory& pdir, uint32_t vaddr, uint32_t count);

int IdentityMap(PageDirectory *pdir, uintptr_t address, uint32_t size);

}
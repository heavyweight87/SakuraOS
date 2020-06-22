#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Multiboot.h"
#include "Paging.h"

namespace MemoryManager {

extern "C" void paging_enable(void);
extern "C" void paging_load_directory(uintptr_t directory);
extern "C" void paging_invalidate_tlb(void);

uintptr_t memory_alloc_identity_page(PageDirectory *pdir);

void Init(Multiboot::Multiboot& multiboot);
void memory_pdir_switch(PageDirectory *pdir);
PageDirectory& memory_kpdir();
uintptr_t MemoryAllocate(PageDirectory& page_directory, size_t size, bool user);

}
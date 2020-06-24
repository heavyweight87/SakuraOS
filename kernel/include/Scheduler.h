#pragma once

#include <stdint.h>
#include "MemoryManager.h"
#include <stdint.h>

namespace Scheduler {

#define PROCESS_NAME_SIZE 32
#define TASK_STACK_SIZE 16384

typedef void (*TaskEntry)();
 
struct Registers
{
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};
 
struct Task 
{
    Registers regs;
    MemoryManager::PageDirectory* pageDirectory;
    Task *next;
};
 
void Init();
void CreateTask(Task& task, void(*)(), uint32_t, uint32_t*);
Task& GetRunningTask();
 
extern void yield(); // Switch task frontend
void Schedule(void);
extern "C" void switchTask(Registers *old, Registers *n); // The function which actually switches
}
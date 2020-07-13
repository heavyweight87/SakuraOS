#pragma once

#include <stdint.h>
#include "memorymanager.h"
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
    Task *next;
};
 
void init();
void createTask(Task& task, uint32_t flags, bool isUSer);
void taskStart(Task& task,  TaskEntry entry);
Task& getRunningTask();
 
void schedule(void);
extern "C" void switchTask(Registers *old, Registers *n); // The function which actually switches
}
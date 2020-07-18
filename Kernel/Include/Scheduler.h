#pragma once

#include <stdint.h>
#include <stdint.h>

namespace Scheduler {

#define PROCESS_NAME_SIZE 32
#define TASK_STACK_SIZE 16384

typedef void (*TaskEntry)();
 
struct Registers
{
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};

enum class TaskState 
{
    None,
    Running,
    Sleep
};
 
struct Task 
{
    Registers regs;
    Task *next;
    char name[50];
    uint64_t wakeupTime;
    TaskState state;
};
 
void init();
Task& createTask(bool isUser);
void taskStart(Task& task,  TaskEntry entry);
Task& getRunningTask();
void disable();
void enable();
 
void schedule(void);
extern "C" void switchTask(Registers *old, Registers *n); // The function which actually switches
}
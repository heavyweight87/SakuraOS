#include "scheduler.h"
#include <stdio.h>
#include <io.h>
#include "syscalls.h"
#include "interrupthandler.h"
#include "libk.h"
#include "arch.h"
#include "kmalloc.h"

namespace Scheduler {
 
static Task *runningTask;
static Task mainTask;
bool schedulerEnabled = true;

static void timerCallback()
{
    schedule();
}

static void ConfigurePit()
{
    uint32_t div = 1193182 / 1000;
    InterruptHandler::registerInterrupt(Interrupts::Timer, timerCallback);
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}
 
void init() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    runningTask = &mainTask; 
    ConfigurePit();      
}
 
Task& createTask(uint32_t flags, bool isUser) 
{
    Task& task = *(Task*)malloc(sizeof(Task));
    task.regs.eax = 0;
    task.regs.ebx = 0;
    task.regs.ecx = 0;
    task.regs.edx = 0;
    task.regs.esi = 0;
    task.regs.edi = 0;
    task.regs.eflags = flags;
    if(isUser)
    {
        task.regs.cr3 = (uint32_t)MemoryManager::CreateUserPageDirectory();
    }
    else
    {
        task.regs.cr3 = (uint32_t) &MemoryManager::GetKerkelPageDirectory();
    }
    task.regs.esp = (uint32_t)MemoryManager::MemoryAllocate(TASK_STACK_SIZE, false) + TASK_STACK_SIZE;
    task.next = 0;
    return task;
}

void taskStart(Task& task,  TaskEntry entry)
{
    task.regs.eip = (uint32_t) entry;
    mainTask.next = &task;
    task.next = &mainTask;
}
 
void yield() 
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void schedule()
{
    if(schedulerEnabled)
    {
        Task *last = runningTask;
        if(runningTask->next)
        {
            runningTask = runningTask->next;
            switchTask(&last->regs, &runningTask->regs);
        }
    }
}

void enable()
{
    schedulerEnabled = true;
}

void disable()
{
    schedulerEnabled = false;
}

Task& getRunningTask()
{
    return *runningTask;
}

}
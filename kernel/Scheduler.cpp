#include "Scheduler.h"
#include <stdio.h>
#include <io.h>

namespace Scheduler {
 
static Task *runningTask;
static Task mainTask;
static Task otherTask;

void SetTimerFrequency(uint16_t hz)
{
    uint32_t div = 1193182 / hz;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}
 
static void otherMain() 
{
    while(1)
    {
        uint32_t mem = TASK_STACK_SIZE*1000;
        printf("Requesting %d bytes\r\n", mem); 
        uint8_t *buffer = (uint8_t*)MemoryManager::MemoryAllocate(MemoryManager::GetKerkelPageDirectory(), mem, false);
        if(buffer)
        {
            for(unsigned int i = 0; i < mem; i++)
            {
                buffer[i] = i;
            }
            MemoryManager::MemoryFree(MemoryManager::GetKerkelPageDirectory(), (uint32_t)buffer, mem);
        }
    }
}
 
void Init() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    runningTask = &mainTask;
    CreateTask(otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;
 
    SetTimerFrequency(1000);
    while(1)
    {
        //kernel backgronud task
      // printf("Switching to otherTask... \n");
    }
}
 
void CreateTask(Task& task, void (*main)(), uint32_t flags, uint32_t *pagedir) 
{
    task.regs.eax = 0;
    task.regs.ebx = 0;
    task.regs.ecx = 0;
    task.regs.edx = 0;
    task.regs.esi = 0;
    task.regs.edi = 0;
    task.regs.eflags = flags;
    task.regs.eip = (uint32_t) main;
    task.regs.cr3 = (uint32_t) &MemoryManager::GetKerkelPageDirectory();
    task.regs.esp = (uint32_t)MemoryManager::MemoryAllocate(TASK_STACK_SIZE, false) + TASK_STACK_SIZE;
    task.next = 0;
}
 
void yield() 
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void Schedule()
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

Task& GetRunningTask()
{
    return *runningTask;
}

}
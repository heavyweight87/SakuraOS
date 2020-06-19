#include <scheduler.h>
#include <stdio.h>
#include "VirtualMemoryManager.h"
#include <io.h>

namespace scheduler {
 
static Task *runningTask;
static Task mainTask;
static Task otherTask;
 
static void otherMain() {
    while(1)
    {
        printf("Hello multitasking world!\r\n"); // Not implemented here...
    }
}
 
void init() 
{

    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
 
    createTask(&otherTask, otherMain, mainTask.regs.eflags, (uint32_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;
 
    runningTask = &mainTask;
    timer_set_frequency(1000);
    while(1)
    {
       printf("Switching to otherTask... \n");
    }
}
 
void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir) 
{
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (uint32_t) main;
    task->regs.cr3 = (uint32_t) pagedir;
    task->regs.esp = (uint32_t)MemoryManager::memory_alloc(&MemoryManager::kpdir, TASK_STACK_SIZE, MEMORY_CLEAR) + 0x4000;
    task->next = 0;
}
 
void yield() 
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void schedule()
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void timer_set_frequency(uint16_t hz)
{
    uint32_t divisor = 1193182 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    printf("Timer started with a frewuency of %d hz\r\n", hz);
}
}
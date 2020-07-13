#include "scheduler.h"
#include <stdio.h>
#include <io.h>
#include "syscalls.h"
#include "interrupthandler.h"
#include "libk.h"
#include "arch.h"

namespace Scheduler {
 
static Task *runningTask;
static Task mainTask;
static Task otherTask;

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
 
static void otherMain() 
{
    while(1)
    {
    }
}
 
void init() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    runningTask = &mainTask;
   // CreateTask(otherTask, otherMain, mainTask.regs.eflags, false);
   // mainTask.next = &otherTask;
    //otherTask.next = &mainTask;
 
    ConfigurePit();
  //  while(1)
    {
        //kernel backgronud task
      //  printf("Switching to otherTask... \n");
  //    memtest();
      
    }
}
 
void createTask(Task& task, uint32_t flags, bool isUser) 
{
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
}

void taskStart(Task& task,  void (*main)())
{
    task.regs.eip = (uint32_t) main;
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
    Task *last = runningTask;
    if(runningTask->next)
    {
        runningTask = runningTask->next;
        switchTask(&last->regs, &runningTask->regs);
    }
}

Task& getRunningTask()
{
    return *runningTask;
}

}
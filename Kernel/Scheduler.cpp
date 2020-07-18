#include "Scheduler.h"
#include <stdio.h>
#include "Syscalls.h"
#include "InterruptHandler.h"
#include "Libk.h"
#include "Arch.h"
#include "kmalloc.h"
#include "List.h"
#include <list>
#include "MemoryManager.h"
#include <vector>

namespace Scheduler {
 
static Task *runningTask;
static Task mainTask;
static Task *taskTail;
bool schedulerEnabled = true;
LinkedList<Task*> taskList;

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

static void fucky()
{
    while(1)
    {
        Libk::printk("fucky0\r\n");
    }
}

static void fucky1()
{
    while(1)
    {
        Libk::printk("fucky1\r\n");
    }
}

static void fucky2()
{
    while(1)
    {
        Libk::printk("fucky2\r\n");
    }
}
 
void init() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    runningTask = &mainTask; 
    sprintf(mainTask.name, "main");
    taskTail = &mainTask;
    Task& task = createTask(false);
    sprintf(task.name, "fucky0");
    taskStart(task, fucky);

    Task& task1 = createTask(false);
    sprintf(task1.name, "fucky1");
    taskStart(task1, fucky1);


    Task& task2 = createTask(false);
    sprintf(task2.name, "fucky2");
    taskStart(task2, fucky2);

    ConfigurePit();
}
 
Task& createTask(bool isUser) 
{
    Task *task = (Task*)kmalloc(sizeof(Task));
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = mainTask.regs.eflags;
    if(isUser)
    {
        task->regs.cr3 = (uint32_t)MemoryManager::createUserPageDirectory();
    }
    else
    {
        task->regs.cr3 = (uint32_t) &MemoryManager::getKerkelPageDirectory();
    }
    task->regs.esp = (uint32_t)MemoryManager::memoryAllocate(TASK_STACK_SIZE, false) + TASK_STACK_SIZE;
    task->next = 0; 
    return *task;
}

void taskStart(Task& task,  TaskEntry entry)
{
    task.regs.eip = (uint32_t) entry;
    taskTail->next = &task;
   taskTail = &task;
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
            Libk::printk("switching to task %s\r\n", runningTask->name);
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
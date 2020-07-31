#include "Scheduler.h"
#include <stdio.h>
#include "InterruptHandler.h"
#include "Libk.h"
#include "Arch.h"
#include "kmalloc.h"
#include "MemoryManager.h"

namespace Kernel {

#define PIT_FREQUENCY_HZ 1000
 
static Scheduler::Task mainTask;
bool schedulerEnabled = true;

uint64_t Scheduler::m_timeMs = 0;
Scheduler::Task *Scheduler::m_taskTail;
Scheduler::Task *Scheduler::m_runningTask;

extern "C" void switchTask(TaskRegisters *old, TaskRegisters *n); // The function which actually switches

void Scheduler::irqCallback(int intNum) 
{
    (void)intNum;
    m_timeMs++;
    Scheduler::schedule();
}

void Scheduler::sleep(uint64_t sleepMs)
{
    m_runningTask->state = TaskState::Sleep;
    m_runningTask->wakeupTime = m_timeMs + sleepMs;
    schedule();
}

static void ConfigurePit()
{
    uint32_t div = 1193182 / PIT_FREQUENCY_HZ;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}

void Scheduler::start() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    InterruptHandler::registerInterrupt(InterruptSource::Timer);
    m_runningTask = &mainTask; 
    sprintf(mainTask.name, "main");
    mainTask.state = TaskState::Running;
    m_taskTail = &mainTask;
    ConfigurePit();
}
 
Scheduler::Task *Scheduler::createTask(bool isUser, void *arg) 
{
    Scheduler::Task *task = (Task*)kmalloc(sizeof(Task));
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
    return task;
}

void Scheduler::taskStart(Scheduler::Task& task, TaskEntry entry)
{
    task.regs.eip = (uint32_t) entry;
    // this task is now the tail
    m_taskTail->next = &task;
    m_taskTail = &task;
    task.next = &mainTask; //loop back to the first task
    task.state = TaskState::Running;
}

Scheduler::Task* Scheduler::goToNextTask()
{
    Task *nextTask = m_runningTask->next;
    while(1)
    {
        if(nextTask == NULL)
        {
            return NULL;
        }
        if(nextTask->state == TaskState::Running || (nextTask->state == TaskState::Sleep && m_timeMs > nextTask->wakeupTime))
        {
            nextTask->state = TaskState::Running;
            return nextTask;
        }
        nextTask = nextTask->next;
    }
}

void Scheduler::schedule()
{
    if(schedulerEnabled)
    {
        Task *last = m_runningTask;
        Task *next = goToNextTask();
        if(next != nullptr)
        {
            m_runningTask = next;
            switchTask(&last->regs, &m_runningTask->regs);
        }
    }
}

void Scheduler::enable()
{
    schedulerEnabled = true;
}

void Scheduler::disable()
{
    schedulerEnabled = false;
}

Scheduler::Task& Scheduler::getRunningTask()
{
    return *m_runningTask;
}

}
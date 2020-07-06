#include "Scheduler.h"
#include <stdio.h>
#include <io.h>
#include "Syscalls.h"
#include "VirtualMemoryManager.h"

namespace Scheduler {
 
static Task *runningTask;
static Task mainTask;
static Task otherTask;

static void SetTimerFrequency(uint16_t hz)
{
    uint32_t div = 1193182 / hz;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}

static void memtest()
{
    //uint32_t mem = TASK_STACK_SIZE*1000;
    uint32_t mem = 8192;
    uint32_t address = 0x40000000;
    printf("Requesting %d bytes\r\n", mem); 
    MemoryManager::PageDirectory * pd = (MemoryManager::PageDirectory*)runningTask->regs.cr3;
    MemoryManager::MemoryMap(*pd, address, 8192*2, false);
    uint8_t *buffer  = (uint8_t*)(address);
    uint8_t *fucky  = (uint8_t*)(address + 4096);
 //   uint32_t phy = 1;//MemoryManager::GetPhysicalAddress(*pd, 0x40000000);
    // uint8_t *buffer = (uint8_t*)MemoryManager::MemoryAllocate(*pd, mem, false);
    if((uint32_t)buffer >= 0x40000000)
        printf("Finally fucking works\r\n");
    if(buffer)
    {
        for(unsigned int i = 0; i < mem; i++)
        {
            buffer[i] = i;
            printf("i = %d\r\n", i);
        }
       // MemoryManager::MemoryFree(*pd, (uint32_t)buffer, mem);
    }
}
 
static void otherMain() 
{
    while(1)
    {
        memtest();
    }
}
 
void Init() 
{
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(mainTask.regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(mainTask.regs.eflags)::"%eax");
    runningTask = &mainTask;
   // CreateTask(otherTask, otherMain, mainTask.regs.eflags, false);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask;
 
    SetTimerFrequency(1000);
  //  int ret = __syscall(SYSCALL_OPEN, 1,2,3,4,5);
    //printf("Does it work? ret = %d\r\n", ret);
  //  while(1)
    {
        //kernel backgronud task
      //  printf("Switching to otherTask... \n");
  //    memtest();
      
    }
}
 
void CreateTask(Task& task, void (*main)(), uint32_t flags, bool isUser) 
{
    task.regs.eax = 0;
    task.regs.ebx = 0;
    task.regs.ecx = 0;
    task.regs.edx = 0;
    task.regs.esi = 0;
    task.regs.edi = 0;
    task.regs.eflags = flags;
    task.regs.eip = (uint32_t) main;
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
 
void yield() 
{
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

void Schedule()
{
    return;
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}

Task& GetRunningTask()
{
    return *runningTask;
}

}
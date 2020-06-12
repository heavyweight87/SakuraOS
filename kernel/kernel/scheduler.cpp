#include <stdio.h>
#include <kernel/scheduler.h>
#include <string.h>
#include "kernel/virtual.h"
#include "kernel/io.h"

static int TID = 0;
static uint32_t ticks = 0;
static Task *running = NULL;
static Task *idle_task;
static Task * garbage_task;

typedef struct __attribute__((packed))
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
} InterruptStackFrame;

void idle()
{
    while(1)
        printf("idle fuck\r\n");
}

void garbage_collector()
{
    while(1)
        printf("garbage!\r\n");
}

void scheduler_setup(Task *main_kernel_task)
{
    running = main_kernel_task;

    timer_set_frequency(1000);
}

void tasking_initialize(void)
{
    printf("Initializing tasking...");

    printf("Spawing system services...");

    running = NULL;

    idle_task = task_spawn(NULL, "Idle", idle, NULL, false);
    task_go(idle_task);
    idle_task->state = TASK_STATE_HANG; 

    Task *kernel_task = task_spawn(NULL, "System", NULL, NULL, false);
    task_go(kernel_task);

    garbage_task = task_spawn(NULL, "GarbageCollector", garbage_collector, NULL, false);
    task_go(garbage_task);

    scheduler_setup(kernel_task);
}

void timer_set_frequency(uint16_t hz)
{
    uint32_t divisor = 1193182 / hz;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    printf("Timer frequency is %dhz.", hz);
}


void task_set_entry(Task *t, TaskEntry entry, bool user)
{
    t->entry = entry;
    t->user = user;
}

uintptr_t task_stack_push(Task *task, const void *value, uint32_t size)
{
    task->stack_pointer -= size;
    memcpy((void *)task->stack_pointer, value, size);

    return task->stack_pointer;
}


Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user)
{
   // ASSERT_ATOMIC;

    Task *t = task_create(parent, name, user);

    task_set_entry(t, entry, user);
    task_stack_push(t, &arg, sizeof(arg));

    return t;
}

Task *task_create(Task *parent, const char *name, bool user)
{
   // ASSERT_ATOMIC;

    Task *task = (Task*)memory_alloc(&kpdir, sizeof(Task), MEMORY_USER | MEMORY_CLEAR);

    task->id = TID++;
    strcpy(task->name, name);
    task->state = TASK_STATE_NONE;

    // Setup memory space
    if (user)
    {
        task->pdir = memory_pdir_create();
    }
    else
    {
        task->pdir = memory_kpdir();
    }


    task->stack = (uint8_t *)memory_alloc(task->pdir, TASK_STACK_SIZE, MEMORY_CLEAR);
    task->stack_pointer = ((uintptr_t)task->stack + TASK_STACK_SIZE - 1);

    return task;
}

void task_go(Task *task)
{
    InterruptStackFrame stackframe;

    stackframe.eflags = 0x202;
    stackframe.eip = (uintptr_t)task->entry;
    stackframe.ebp = ((uintptr_t)task->stack + TASK_STACK_SIZE);

    // TODO: userspace task
    stackframe.cs = 0x08;
    stackframe.ds = 0x10;
    stackframe.es = 0x10;
    stackframe.fs = 0x10;
    stackframe.gs = 0x10;

    task_stack_push(task, &stackframe, sizeof(InterruptStackFrame));

   // atomic_begin();
    task->state = TASK_STATE_RUNNING;
  //  atomic_end();
}


uintptr_t schedule(uintptr_t esp)
{
    running->stack_pointer = esp;
    if(running == idle_task)
    {
        running = garbage_task;
    }
    else
    {
        running = idle_task;
    }
    memory_pdir_switch(running->pdir);
    return running->stack_pointer;
}


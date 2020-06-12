#pragma once

#include "kernel/task.h"
#include <stdint.h>
#include "kernel/paging.h"

#define PROCESS_NAME_SIZE 32

struct Task;

typedef void (*TaskEntry)();

typedef struct Task
{
    int id;
    bool user;
    char name[PROCESS_NAME_SIZE]; // Friendly name of the process

    TaskState state;

    uintptr_t stack_pointer;
    void *stack;     // Kernel stack
    TaskEntry entry; // Our entry point
    char fpu_registers[512];
    PageDirectory *pdir; // Page directory

    int exit_value;
} Task;

#define TASK_STACK_SIZE 16384

void tasking_initialize(void);
Task *task_create(Task *parent, const char *name, bool user);
Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);
PageDirectory *memory_pdir_create(void);
void timer_set_frequency(uint16_t hz);
void task_go(Task *t);
uintptr_t schedule(uintptr_t);
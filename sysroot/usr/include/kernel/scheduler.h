#pragma once

#include "kernel/task.h"
#include <stdint.h>
#include "kernel/paging.h"

#define PROCESS_NAME_SIZE 32

struct Task;

typedef void (*TaskEntry)();

typedef struct
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
} Task2;

#define TASK_STACK_SIZE 16384

void tasking_initialize(void);
Task *task_create(Task *parent, const char *name, bool user);
Task *task_spawn(Task *parent, const char *name, TaskEntry entry, void *arg, bool user);
PageDirectory *memory_pdir_create(void);
void timer_set_frequency(uint16_t hz);
void task_go(Task *t);
void schedule(void);

typedef struct __attribute__((packed))
{
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intno, err;
    uint32_t eip, cs, eflags;
} InterruptStackFrame;





#include <stdint.h>
 
extern void initTasking();
 
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;
 
typedef struct Task {
    Registers regs;
    struct Task *next;
} Task;
 
extern void initTasking();
extern void createTask(Task*, void(*)(), uint32_t, uint32_t*);
 
extern void yield(); // Switch task frontend
extern "C" void switchTask(Registers *old, Registers *n); // The function which actually switches
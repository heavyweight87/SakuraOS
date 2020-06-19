#pragma once

#include "task.h"
#include <stdint.h>
#include "Paging.h"
#include <stdint.h>

namespace scheduler {

#define PROCESS_NAME_SIZE 32
#define TASK_STACK_SIZE 16384

typedef void (*TaskEntry)();
 
typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;
 
typedef struct Task {
    Registers regs;
    Task *next;
}Task;
 
void init();
void createTask(Task*, void(*)(), uint32_t, uint32_t*);
void timer_set_frequency(uint16_t hz);
 
extern void yield(); // Switch task frontend
void schedule(void);
extern "C" void switchTask(Registers *old, Registers *n); // The function which actually switches
}
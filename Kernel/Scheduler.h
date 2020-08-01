#pragma once

#include <stdint.h>
#include <stdint.h>
#include "InterruptHandler.h"

namespace Kernel {

#define PROCESS_NAME_SIZE 32
#define TASK_STACK_SIZE 0x1000
 
struct TaskRegisters
{
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};

enum class TaskState 
{
    None,
    Running,
    Sleep
};

class Scheduler : InterruptHandler {
    public:
        struct Task 
        {
            TaskRegisters regs;
            Task *next;
            char name[PROCESS_NAME_SIZE];
            uint64_t wakeupTime;
            TaskState state;
        };
        typedef void (*TaskEntry)(void *);
        void start();
        static Task *createTask(bool isUser, void *arg);
        static void taskStart(Task& task,  TaskEntry entry);
        static Task& getRunningTask();
        static void disable();
        static void enable();
        static void schedule(void);
        static void sleep(uint64_t sleepMs);
    private:
        static Task *m_runningTask;
        static Task *m_taskTail;
        static Task* goToNextTask();
        static uint64_t m_timeMs;
        void irqCallback(int intNum) override;
};

}
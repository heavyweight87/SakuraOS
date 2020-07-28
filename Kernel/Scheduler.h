#pragma once

#include <stdint.h>
#include <stdint.h>

namespace Kernel {

#define PROCESS_NAME_SIZE 32
#define TASK_STACK_SIZE 16384
 
struct Registers
{
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
};

enum class TaskState 
{
    None,
    Running,
    Sleep
};

class Scheduler {
    public:
        struct Task 
        {
            Registers regs;
            Task *next;
            char name[50];
            uint64_t wakeupTime;
            TaskState state;
        };
        typedef void (*TaskEntry)();
        void start();
        static Task& createTask(bool isUser);
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
};

}
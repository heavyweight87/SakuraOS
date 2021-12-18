#include "Libk.h"
#include "Devices/TTY.h"
#include <Serial.h>
#include "Multiboot.h"
#include <string.h>
#include "IDT.h"
#include "GDT.h"
#include "Scheduler.h"
#include "MemoryManager.h"
#include "Keyboard.h"
#include "Devices/PCIEnumerator.h"

extern void (*start_ctors)(void) __attribute__((weak));
extern void (*end_ctors)(void) __attribute__((weak));

//#define TASKTEST

#ifdef TASKTEST
static void task1(void *arg)
{
    while(1)
    {
        Libk::printk("task1\r\n");
        Kernel::Scheduler::sleep(3000);
    }
}

static void task2(void *arg)
{
    Kernel::Scheduler::sleep(1000);
    while(1)
    {
        Libk::printk("task2\r\n");
        Kernel::Scheduler::sleep(3000);
    }
}

static void task3(void *arg)
{
    Kernel::Scheduler::sleep(2000);
    while(1)
    {
        Libk::printk("task3\r\n");
        Kernel::Scheduler::sleep(3000);
    }
}

static void taskTest()
{
    Kernel::Scheduler::Task *t1 = Kernel::Scheduler::createTask(false, NULL);
    Libk::sprintf(t1->name, "task1");

    Kernel::Scheduler::Task *t2 = Kernel::Scheduler::createTask(false, NULL);
    Libk::sprintf(t2->name, "task2");

    Kernel::Scheduler::Task *t3 = Kernel::Scheduler::createTask(false, NULL);
    Libk::sprintf(t3->name, "task3");

    Kernel::Scheduler::taskStart(*t1, task1);
    Kernel::Scheduler::taskStart(*t2, task2);
    Kernel::Scheduler::taskStart(*t3, task3);
}
#endif

static void idleTask()
{
    while(1)
    {
        halt();
    }
}

static void initGlobalConstructors()
{
    // Constructor list is defined in the linker script.
    // The .ctors section is just an array of function pointers.
    // iterate through, calling each in turn.
    uintptr_t *iterator = reinterpret_cast<uintptr_t*>(&start_ctors);
    while (iterator < reinterpret_cast<uintptr_t*>(&end_ctors))
    {
        void (*fp)(void) = reinterpret_cast<void (*)(void)>(*iterator);
        fp();
        iterator++;
    }
}

extern "C" int kernel_main(uint32_t magic, Kernel::MultibootInfo *mbinfo) 
{
    Kernel::IDT idt;
    Kernel::Scheduler scheduler;
    gdt::init();
	Devices::TTY tty; //enable early so we have debugging
	Serial::Init();
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        Libk::printk("Booted with an unsupported bootloader %d\r\n", magic);
        return 0;
    }

    Kernel::Multiboot multiboot(*mbinfo);
    idt.init();
    MemoryManager::init(multiboot); 
    initGlobalConstructors(); //call global constructors once there is paging and malloc etc
    scheduler.start();
    
    tty.open();
    Libk::setTTY(&tty);
    Libk::printk("Sakura 1.0.0\r\r");
    Devices::PCIEnumerator pciEnumerator;
  //  taskTest();
    //multiboot.loadModules();  //dont load modules yet... still need some work on syscalls
    pciEnumerator.enumerate();
    idleTask();
    return 0;
}

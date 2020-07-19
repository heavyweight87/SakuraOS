#include "Libk.h"
#include <tty.h>
#include <Serial.h>
#include "Multiboot.h"
#include <string.h>
#include "IDT.h"
#include "GDT.h"
#include "Scheduler.h"
#include "MemoryManager.h"

extern void (*start_ctors)(void) __attribute__((weak));
extern void (*end_ctors)(void) __attribute__((weak));

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

extern "C" int kernel_main(uint32_t magic, Multiboot::MultibootInfo *mbinfo) 
{
    gdt::init();
	terminal_init(); //enable early so we have debugging
	Serial::Init();
    Libk::printk("Sakura 1.0.0\r\n");
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        Libk::printk("Booted with an unsupported bootloader %d\r\n", magic);
        return 0;
    }

    Multiboot::Multiboot multiboot(*mbinfo);
    MemoryManager::init(multiboot); 
    IDT::init();
    initGlobalConstructors(); //call global constructors once there is paging and malloc etc
    Scheduler::init();
//    multiboot.loadModules();  //dont load modules yet... still need some work on syscalls
    while(1);
    return 0;
}

#include "Libk.h"
#include <tty.h>
#include <Serial.h>
#include "Multiboot.h"
#include <string.h>
#include "IDT.h"
#include "GDT.h"
#include "Scheduler.h"
#include "MemoryManager.h"

extern "C" int kernel_main(uint32_t magic, Multiboot::MultibootInfo *mbinfo) 
{
    gdt::init();
	terminal_init(); //enable early so we have debugging
	Serial::Init();
    Libk::printk("Sakura 1.0.0 %x\r\n", 55);
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        Libk::printk("Booted with an unsupported bootloader %d\r\n", magic);
        return 0;
    }

    Multiboot::Multiboot multiboot(*mbinfo);
    MemoryManager::Init(multiboot); 
    IDT::init();
    Scheduler::init(); 
    multiboot.LoadModules();  
    while(1);
    return 0;
}

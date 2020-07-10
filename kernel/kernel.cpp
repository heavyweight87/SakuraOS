#include <stdio.h>   
#include <tty.h>
#include <serial.h>
#include <io.h> 
#include "multiboot.h"
#include <string.h>
#include <interrupts.h>
#include <gdt.h>
#include "scheduler.h"
#include "memorymanager.h"

extern "C" int kernel_main(uint32_t magic, Multiboot::MultibootInfo *mbinfo) 
{
    gdt::init();
	terminal_init(); //enable early so we have debugging
	serial_init();
    printf("Sakura 1.0.0\r\n");
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf("Booted with an unsupported bootloader %d\r\n", magic);
        return 0;
    }
    Multiboot::Multiboot multiboot(*mbinfo);
    MemoryManager::Init(multiboot); 
    Interrupts::Init();
    Scheduler::Init(); 
    multiboot.LoadModules(); 
    return 0;
}

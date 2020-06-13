#include <stdio.h>   
#include <kernel/tty.h>
#include <kernel/serial.h>
#include <kernel/io.h> 
#include <kernel/multiboot.h>
#include <string.h>
#include <kernel/interrupts.hpp>
#include <kernel/gdt.hpp>
#include <kernel/scheduler.h>
#include <kernel/paging.h>

#define MULTIBOOT_MAGIC 0x2BADB002
typedef void (*call_module_t)(void);


uint8_t kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',       /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', ',', '.', '/',   0,                    /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

static void load_modules(multiboot_info_t *mbinfo)
{
    unsigned long moduleNum = 0;
    module_t *module = (module_t*)mbinfo->mods_addr;
    printf("Loading %d modules\r", mbinfo->mods_count);
    while(moduleNum < mbinfo->mods_count)
    {
        call_module_t start_program = (call_module_t) module->mod_start;
        module++;
        start_program();
    }
}

extern "C" int kernel_main(uint32_t magic, multiboot_info_t *mbinfo) 
{
    gdt::init();
	terminal_initialize();
	serial_init();
    printf("Hello, kernel World!\r\n");
    if(magic != MULTIBOOT_MAGIC)
    {
        printf("Booted with an unsupported bootloader\r\n");
        return 0;
    }
    printf("Multiboot magic ok!\r");

    if(mbinfo->mods_count)
    {
        load_modules(mbinfo);
    }
    paging_init(mbinfo);
    switch_to_virtual();
    printf("Paging enabled!");
    interrupts::init();
    initTasking();
    return 0;
}


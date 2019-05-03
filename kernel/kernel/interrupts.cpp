#include <stdio.h>
#include <stdint.h>
#include <kernel/interrupts.hpp>
#include <stdint.h>
#include <kernel/io.h>
#include <string.h>


typedef struct  __attribute__((packed)) 
{
    uint16_t length;
    void*    base;
}idtr_t;

typedef struct __attribute__((packed))
{
    uint8_t type    : 4;
    uint8_t zero    : 1;
    uint8_t dpl     : 2;
    uint8_t present : 1;
}idt_flags_t; 

typedef struct __attribute__((packed))
{
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t  zero;
    idt_flags_t flags;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
}idt_entry_t; 

idt_entry_t idt[47];

extern "C" void irq_handler()
{
    printf("irq works!!!!");
}


extern "C" void init_table()
{
    extern int irq0();
    extern int irq1();
    uint64_t irqAddress = (uint64_t)irq0;
    idtr_t idtr;
    idtr.base = idt;
    idtr.length = sizeof(idt_entry_t) * 47;
    memset(idt, 0, sizeof(idt));


    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 40);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    for(int x = 0; x < 47; x++)
    {
        idt[x].offset_low = irqAddress;
        idt[x].segment_selector = 0x8; //interrupt gate
        idt[x].offset_middle = irqAddress >> 16;
        idt[x].offset_high = irqAddress >> 32;
        idt[x].flags.type = 0xE;
        idt[x].flags.present = 1;
        idt[x].flags.dpl = 0;
    }

    /*idt[32].offset_low = irqAddress;
    idt[32].segment_selector = 0x18; //interrupt gate
    idt[32].offset_middle = irqAddress >> 16;
    idt[32].offset_high = irqAddress >> 32;
    idt[32].flags.type = 0xE;
    idt[32].flags.present = 1;
    idt[32].flags.dpl = 0;

    irqAddress = (uint64_t)irq1;
    idt[33].offset_low = irqAddress;
    idt[33].segment_selector = 0x18; //interrupt gate
    idt[33].offset_middle = irqAddress >> 16;
    idt[33].offset_high = irqAddress >> 32;
    idt[33].flags.type = 0xE;
    idt[33].flags.present = 1;
    idt[33].flags.dpl = 0;*/

    asm ( "lidt %0" : : "m"(idtr) );  // let the compiler choose an addressing mode
    asm volatile("sti" : : );
}

void interrupts::init()
{    
   
    init_table();
    printf("Initializing interrupts\r");

}
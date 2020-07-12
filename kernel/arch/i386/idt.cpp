#include "libk.h"
#include <stdint.h>
#include <io.h>
#include "scheduler.h"
#include "syscalls.h"
#include "arch.h"

namespace IDT {

extern "C" int idt_load(unsigned long);
extern "C" int irq0();
extern "C" int irq1();
extern "C" int irq2();
extern "C" int irq3();
extern "C" int irq4();
extern "C" int irq5();
extern "C" int irq6();
extern "C" int irq7();
extern "C" int irq8();
extern "C" int irq9();
extern "C" int irq10();
extern "C" int irq11();
extern "C" int irq12();
extern "C" int irq13();
extern "C" int irq14();
extern "C" int irq15();
extern "C" int irq15();
extern "C" int irq32();
extern "C" int irq128();

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


extern "C" void interruptHandler(Registers *regs)
{
    outb(0xA0, 0x20);
    outb(0x20, 0x20); //EOI
    if(regs->int_no == 128)
    {
        regs->eax = Syscalls::Handle((Syscalls::Syscall)regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
    }
    else if(regs->int_no == 32)
    {
        Scheduler::Schedule();
    }
}

struct IDT_entry
{
    unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[256];

static void ConfigureGate(std::uintptr_t offset, int gateNum)
{
    IDT[gateNum].offset_lowerbits = offset & 0xffff;
    IDT[gateNum].selector = 0x08; 
    IDT[gateNum].zero = 0;
    IDT[gateNum].type_attr = 0x8E;
    IDT[gateNum].offset_higherbits = (offset & 0xffff0000) >> 16;
}


void init_table()
{
    unsigned long idt_address;
    unsigned long idt_ptr[2];
    ConfigureGate((uintptr_t)irq32, 32);
    ConfigureGate((uintptr_t)irq128, 128);



    /* fill the IDT descriptor */
    idt_address = (unsigned long)IDT ;
    idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16 ;
    idt_load((unsigned long)idt_ptr);
    enableInterrupts();
}


void Init()
{    
        /* remapping the PIC */
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
    init_table();
}

}
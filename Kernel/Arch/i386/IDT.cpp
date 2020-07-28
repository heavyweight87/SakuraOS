#include "Libk.h"
#include <stdint.h>
#include "InterruptHandler.h"
#include "Syscalls.h"
#include "Arch.h"
#include "IDT.h"

namespace Kernel {

#define NUM_IDT_ENTRIES 256

#define PIC1		    0x20		// IO base address for master PIC
#define PIC2		    0xA0		// IO base address for slave PIC 
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	    (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	    (PIC2+1)
#define PIC_EOI		    0x20		// End-of-interrupt command code 

struct IDT_entry
{
    std::uint16_t offsetLower;
	std::uint16_t selector;
	std::uint8_t zero;
	std::uint8_t typeAttribute;
	std::uint16_t offsethigher;
};

struct IDT_entry IDTTable[NUM_IDT_ENTRIES];

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
extern "C" int irq17();
extern "C" int irq32();
extern "C" int irq33();
extern "C" int irq128();

extern "C" void interruptHandler(Registers *regs)
{
    if(regs->int_no >= 40)
    {
		outb(PIC2_COMMAND,PIC_EOI);
    }
    else
    {
	    outb(PIC1_COMMAND,PIC_EOI);
    }
    if(regs->int_no < 32)
    {
        //exception handler
        Libk::printk("Exception occured! %d\r\n", regs->int_no);
        while(1);
    }
    else if(regs->int_no < 48)
    {
        InterruptHandler::InterruptHandler::interruptHandler(static_cast<InterruptSource>(regs->int_no));
    }
    else if(regs->int_no == 128)
    {
        regs->eax = Syscalls::Handle((Syscalls::Syscall)regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);
    }
}

static void configureGate(std::uintptr_t offset, int gateNum)
{
    IDTTable[gateNum].offsetLower = offset & 0xffff;
    IDTTable[gateNum].selector = 0x08; 
    IDTTable[gateNum].zero = 0;
    IDTTable[gateNum].typeAttribute = 0x8E;
    IDTTable[gateNum].offsethigher = (offset & 0xffff0000) >> 16;
}


static void configureIdt()
{
    configureGate((uintptr_t)irq0, 0);
    configureGate((uintptr_t)irq1, 1);
    configureGate((uintptr_t)irq3, 2);
    configureGate((uintptr_t)irq4, 4);
    configureGate((uintptr_t)irq5, 5);
    configureGate((uintptr_t)irq6, 6);
    configureGate((uintptr_t)irq7, 7);
    configureGate((uintptr_t)irq8, 8);
    configureGate((uintptr_t)irq10, 10);
    configureGate((uintptr_t)irq11, 11);
    configureGate((uintptr_t)irq12, 12);
    configureGate((uintptr_t)irq13, 13);
    configureGate((uintptr_t)irq14, 14);
    configureGate((uintptr_t)irq17, 17);
    configureGate((uintptr_t)irq32, 32);
    configureGate((uintptr_t)irq33, 33);
    configureGate((uintptr_t)irq128, 128);
}


void IDT::init()
{    
    std::uint32_t idt_address;
    std::uint32_t idt_ptr[2];
    // Remap the PIC 
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
    configureIdt();

        /* fill the IDT descriptor */
    idt_address = (unsigned long)IDTTable;
    idt_ptr[0] = (sizeof (struct IDT_entry) * NUM_IDT_ENTRIES) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16 ;
    idt_load((unsigned long)idt_ptr);
    enableInterrupts();
}

}
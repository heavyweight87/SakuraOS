#include <stdio.h>
#include <stdint.h>
#include <interrupts.h>
#include <io.h>
#include <string.h>
#include "Scheduler.h"
#include "Syscalls.h"


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

idt_entry_t idt[256];

static uint8_t kbdus[128] = {
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

extern "C"
{

void irq0_handler() {
	  // TODO: Add timer handler
	  outb(0x20, 0x20);
    Scheduler::Schedule();
}

void irq1_handler(void) {
	// TODO: Use pointers to add later a getch() function in stdio

	uint8_t keycode;

	keycode = inb(0x60);
    if(!(keycode & 0x80))
    {
       // printf("%c", kbdus[keycode]);
    }

	outb(0x20, 0x20);
}

void irq2_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq3_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq4_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq5_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq6_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq7_handler(void) {
  outb(0x20, 0x20); //EOI
}

void irq8_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq9_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq10_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq11_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq12_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq13_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq14_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

void irq15_handler(void) {
  outb(0xA0, 0x20);
  outb(0x20, 0x20); //EOI
}

struct regs {
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

void irq128_handler(struct regs *r) 
{
  printf("syscall = %d, p1 = %d, p2 = %d, p3 = %d, p4 = %d, p5 = %d\r\n", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
  r->eax = 55;
}

}

struct IDT_entry{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[256];


extern "C" void init_table()
{
    extern int load_idt(unsigned long);
    extern int irq0();
    extern int irq1();
    extern int irq2();
    extern int irq3();
    extern int irq4();
    extern int irq5();
    extern int irq6();
    extern int irq7();
    extern int irq8();
    extern int irq9();
    extern int irq10();
    extern int irq11();
    extern int irq12();
    extern int irq13();
    extern int irq14();
    extern int irq15();
    extern int irq128();

	  unsigned long irq0_address;
    unsigned long irq1_address;
    unsigned long irq2_address;
    unsigned long irq3_address;
    unsigned long irq4_address;
    unsigned long irq5_address;
    unsigned long irq6_address;
    unsigned long irq7_address;
    unsigned long irq8_address;
    unsigned long irq9_address;
    unsigned long irq10_address;
    unsigned long irq11_address;
    unsigned long irq12_address;
    unsigned long irq13_address;
    unsigned long irq14_address;
    unsigned long irq15_address;
    unsigned long irq128_address;
	  unsigned long idt_address;
	  unsigned long idt_ptr[2];

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

    irq0_address = (unsigned long)irq0; 
    IDT[32].offset_lowerbits = irq0_address & 0xffff;
    IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[32].zero = 0;
    IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;

    irq1_address = (unsigned long)irq1; 
    IDT[33].offset_lowerbits = irq1_address & 0xffff;
    IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[33].zero = 0;
    IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;

    irq2_address = (unsigned long)irq2; 
    IDT[34].offset_lowerbits = irq2_address & 0xffff;
    IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[34].zero = 0;
    IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;

    irq3_address = (unsigned long)irq3; 
    IDT[35].offset_lowerbits = irq3_address & 0xffff;
    IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[35].zero = 0;
    IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;

    irq4_address = (unsigned long)irq4; 
    IDT[36].offset_lowerbits = irq4_address & 0xffff;
    IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[36].zero = 0;
    IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;

    irq5_address = (unsigned long)irq5; 
    IDT[37].offset_lowerbits = irq5_address & 0xffff;
    IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[37].zero = 0;
    IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;

    irq6_address = (unsigned long)irq6; 
    IDT[38].offset_lowerbits = irq6_address & 0xffff;
    IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[38].zero = 0;
    IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;

    irq7_address = (unsigned long)irq7; 
    IDT[39].offset_lowerbits = irq7_address & 0xffff;
    IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[39].zero = 0;
    IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;

    irq8_address = (unsigned long)irq8; 
    IDT[40].offset_lowerbits = irq8_address & 0xffff;
    IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[40].zero = 0;
    IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;

    irq9_address = (unsigned long)irq9; 
    IDT[41].offset_lowerbits = irq9_address & 0xffff;
    IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[41].zero = 0;
    IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;

    irq10_address = (unsigned long)irq10; 
    IDT[42].offset_lowerbits = irq10_address & 0xffff;
    IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[42].zero = 0;
    IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;

    irq11_address = (unsigned long)irq11; 
    IDT[43].offset_lowerbits = irq11_address & 0xffff;
    IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[43].zero = 0;
    IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;

    irq12_address = (unsigned long)irq12; 
    IDT[44].offset_lowerbits = irq12_address & 0xffff;
    IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[44].zero = 0;
    IDT[44].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;

    irq13_address = (unsigned long)irq13; 
    IDT[45].offset_lowerbits = irq13_address & 0xffff;
    IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[45].zero = 0;
    IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;

    irq14_address = (unsigned long)irq14; 
    IDT[46].offset_lowerbits = irq14_address & 0xffff;
    IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[46].zero = 0;
    IDT[46].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;

    irq15_address = (unsigned long)irq15; 
    IDT[47].offset_lowerbits = irq15_address & 0xffff;
    IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[47].zero = 0;
    IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;

    irq128_address = (unsigned long)irq128; 
    IDT[128].offset_lowerbits = irq128_address & 0xffff;
    IDT[128].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[128].zero = 0;
    IDT[128].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[128].offset_higherbits = (irq128_address & 0xffff0000) >> 16;

    /* fill the IDT descriptor */
    idt_address = (unsigned long)IDT ;
    idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16 ;
    load_idt((unsigned long)idt_ptr);
    asm volatile("sti" : : );
}


void interrupts::init()
{    
   
    init_table();
    printf("Initializing interrupts\r");

}
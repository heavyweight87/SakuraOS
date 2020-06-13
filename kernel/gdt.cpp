// Used for creating GDT segment descriptors in 64-bit integer form.
 
#include <stdio.h>
#include <stdint.h>
#include <kernel/gdt.hpp>


typedef struct __attribute__((packed))
{
	uint32_t prev_tss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} TSS;

static TSS tss = {};
 
// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
 
#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR
 
#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_DATA_RDWR


typedef struct __attribute__((packed))
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
}gdt_entry_t;

struct gdt_ptr
{
    uint16_t limit;
    void *base;
} __attribute__((packed));

gdt_entry_t gdTable[5] __attribute__((aligned(8)));
struct gdt_ptr gp;                     


void create_descriptor(uint32_t base, uint32_t limit, uint16_t flag, gdt_entry_t &desc)
{
    uint64_t *descriptor = (uint64_t*)&desc;
    // Create the high 32 bit segment
    *descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    *descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    *descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    *descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    *descriptor <<= 32;
 
    // Create the low 32 bit segment
    *descriptor |= base  << 16;                       // set base bits 15:0
    *descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0
}

extern "C" void reload()
{
    extern void gdt_flush(struct gdt_ptr* gdt_ptr_addr);
    gdt_flush(&gp);
   // reloadSegs();
}

void gdt::init()
{
    tss.ss0 = 0x10;
    tss.esp0 = 0;
    tss.cs = 0x0b;
    tss.ss = 0x13;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;

    create_descriptor(0, 0, 0, gdTable[0]);
    create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL0), gdTable[1]);
    create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL0), gdTable[2]);
    create_descriptor(0, 0x000FFFFF, (GDT_CODE_PL3), gdTable[3]);
    create_descriptor(0, 0x000FFFFF, (GDT_DATA_PL3), gdTable[4]);
    create_descriptor((uint32_t)&tss, sizeof(TSS), 99, gdTable[5]);

    gp.base = gdTable;
    gp.limit =  (sizeof(gdt_entry_t) * 6) - 1;
    reload();
      
}
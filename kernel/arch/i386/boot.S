.global load_gdt
# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data


# GDT with a NULL Descriptor, a 32-Bit code Descriptor
# and a 32-bit Data Descriptor
gdt_start:
gdt_null:
    .long 0x0
    .long 0x0

gdt_code:
    .short 0xffff
    .short 0x0
    .byte 0x0
    .byte 0b10011010
    .byte 0b11001111
    .byte 0x0

gdt_data:
    .short 0xffff
    .short 0x0
    .byte 0x0
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x0
gdt_end:

# GDT descriptor record
gdtr:
    .short gdt_end - gdt_start - 1
    .long gdt_start

CODE_SEL = gdt_code - gdt_start
DATA_SEL = gdt_data - gdt_start

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
    push %ebx  # push multiboot info
	push %eax  # push magic number
	# Call the global constructors.
	call _init
	# call load_gdt
	# Transfer control to the main kernel
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start


.global load_gdt
# Load GDT and set selectors for a flat memory model
load_gdt:
    lgdt (gdtr)
    ljmp $CODE_SEL, $.setcs             # Set CS selector with far JMP
.setcs:
    mov $DATA_SEL, %eax                 # Set the Data selectors to defaults
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %ss
    mov %eax, %fs
    mov %eax, %gs
    ret

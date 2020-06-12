.section .text
.global paging_enable
paging_enable:
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    ret

.section .text
.global paging_disable
paging_disable:
    mov %cr0, %eax
    and $0x7fffffff, %eax
    mov %eax, %cr0

.section .text
.global paging_load_directory
paging_load_directory:
    mov 4(%esp), %eax
    mov %eax, %cr3
    ret

.section .text
.global paging_invalidate_tlb
paging_invalidate_tlb:
    mov  %cr3, %eax
    mov  %eax, %cr3
    ret
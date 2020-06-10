.global paging_invalidate_tlb
paging_invalidate_tlb:
    mov %eax, %cr3
    mov %cr3, %eax
    ret

.global paging_enable
paging_enable:
    mov %cr0, %eax
    orl $0x80000000, %eax
    mov %cr0, %eax
    ret

.global paging_disable
paging_disable:
    mov %eax, %cr0
    andl $0x7fffffff, %eax
    mov %cr0, %eax
    ret

.global paging_load_directory
paging_load_directory:
    mov %eax, 4(%esp)
    mov %cr3, %eax
    ret
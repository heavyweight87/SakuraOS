.section .text
.global enablePaging
enablePaging:
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    ret

.section .text
.global disablePaging
disablePaging:
    mov %cr0, %eax
    and $0x7fffffff, %eax
    mov %eax, %cr0

.section .text
.global changePageDirectory
changePageDirectory:
    mov 4(%esp), %eax
    mov %eax, %cr3
    ret

.section .text
.global flushCurrentPageDirectory
flushCurrentPageDirectory:
    # this flushes the paging TLB 
    # should be called when pages
    # are mapped or freed
    mov  %cr3, %eax
    mov  %eax, %cr3
    ret
    
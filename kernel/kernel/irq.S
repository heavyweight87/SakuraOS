.global irq0
.global irq1
.global reloadSegs

irq0:
  pusha
  call irq_handler
  popa
  iret

irq1:
  pusha
  popa
  iret

reloadSegs:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %ss
    mov %ax, %gs
    ljmp $0x08, $.flush

.flush:
    ret
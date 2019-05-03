.global _start
.extern kernel_main
.extern exit
.extern _init
.extern _fini
.extern btos_open_std_streams
_start:
call btos_open_std_streams
call _init
call kernel_main
push %eax
call _fini
pop %ebx
mov $0x0805, %eax
int $0x80
.wait:
jmp .wait
[bits 32]

global kernel_start
extern kernel_main

section .text
kernel_start:
    mov esp, stack_top
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 4096
stack_top:

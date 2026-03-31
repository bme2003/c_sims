[bits 32]

global kernel_start
extern kernel_main

section .text
kernel_start:
    ; Switch to the kernel stack and forward the boot info pointer to C.
    mov esp, stack_top
    push esi
    call kernel_main
    add esp, 4

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 4096
stack_top:

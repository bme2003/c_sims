[bits 32]

global irq0
global irq1

extern isr_common_handler

irq0:
    ; Tag the timer IRQ before entering the shared interrupt stub.
    push dword 0
    push dword 32
    jmp irq_common_stub

irq1:
    ; Tag the keyboard IRQ before entering the shared interrupt stub.
    push dword 0
    push dword 33
    jmp irq_common_stub

irq_common_stub:
    ; Save machine state, switch to kernel data segments, and call C code.
    push ds
    push es
    push fs
    push gs
    pusha

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call isr_common_handler
    add esp, 4

    popa
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 8
    iretd

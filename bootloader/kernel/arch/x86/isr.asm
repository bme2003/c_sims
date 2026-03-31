[bits 32]

global isr0
global isr13
global isr14

extern isr_common_handler

%macro ISR_NOERR 1
%1:
    push dword 0
    push dword %2
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
%1:
    push dword %2
    jmp isr_common_stub
%endmacro

isr0:
    ; Build a synthetic error-code frame for divide-by-zero.
    push dword 0
    push dword 0
    jmp isr_common_stub

isr13:
    ; Preserve the CPU-supplied error code and tag the interrupt number.
    push dword 13
    jmp isr_common_stub

isr14:
    ; Preserve the CPU-supplied error code and tag the interrupt number.
    push dword 14
    jmp isr_common_stub

isr_common_stub:
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

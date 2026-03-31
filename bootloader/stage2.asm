[org 0x8000]
[bits 16]

%define KERNEL_LOAD_SEGMENT 0x1000
%define KERNEL_LOAD_OFFSET  0x0000
%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS      10
%endif
%define KERNEL_START_SECTOR 6
%define KERNEL_LOAD_ADDRESS 0x10000

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

stage2_start:
    ; Set up a clean real-mode environment and collect basic boot info.
    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x9000
    sti

    mov [boot_drive], dl
    int 0x12
    mov [boot_memory_kb], ax

    mov si, stage2_message
    call print_string

    mov ax, KERNEL_LOAD_SEGMENT
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0x00
    mov cl, KERNEL_START_SECTOR
    mov dh, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc kernel_load_error

    cli
    call enable_a20
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:protected_mode_start

kernel_load_error:
    ; Stop here if the kernel could not be loaded from disk.
    mov si, kernel_error_message
    call print_string
    jmp $

print_string:
    ; Print a null-terminated string with BIOS teletype output.
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

enable_a20:
    ; Enable access above 1 MiB before entering protected mode.
    in al, 0x92
    or al, 0x02
    out 0x92, al
    ret

stage2_message db ' Stage 2 loaded', 0
kernel_error_message db ' Kernel load failed', 0

gdt_start:
gdt_null:
    dq 0x0000000000000000
gdt_code:
    dq 0x00CF9A000000FFFF
gdt_data:
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
protected_mode_start:
    ; Load protected-mode segments and jump into the 32-bit kernel entry.
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00

    mov esi, boot_info
    mov eax, KERNEL_LOAD_ADDRESS
    jmp eax

align 4
boot_info:
boot_drive db 0
boot_reserved db 0
boot_memory_kb dw 0
boot_kernel_load_addr dd KERNEL_LOAD_ADDRESS

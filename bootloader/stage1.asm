[org 0x7C00]
[bits 16]

%define STAGE2_LOAD_SEGMENT 0x0800
%define STAGE2_LOAD_OFFSET  0x0000
%define STAGE2_SECTORS      4

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, stage1_message
    call print_string

    mov ax, STAGE2_LOAD_SEGMENT
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, STAGE2_SECTORS
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    mov dl, [boot_drive]
    jmp STAGE2_LOAD_SEGMENT:STAGE2_LOAD_OFFSET

disk_error:
    mov si, disk_error_message
    call print_string
    jmp $

print_string:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp print_string
.done:
    ret

stage1_message db 'Stage 1 bootloader', 0
disk_error_message db ' Disk read failed', 0
boot_drive db 0

times 510 - ($ - $$) db 0
dw 0xAA55

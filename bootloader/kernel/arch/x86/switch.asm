[bits 32]

global context_enter_first_task
global context_switch

; Load the saved context for the first task and jump into its entry point.
context_enter_first_task:
    mov edx, [esp + 4]

    mov edi, [edx + 0]
    mov esi, [edx + 4]
    mov ebp, [edx + 8]
    mov ebx, [edx + 12]

    push dword [edx + 36]
    popfd

    mov esp, [edx + 28]
    jmp dword [edx + 32]

; Save the old cooperative task context and resume the next one.
context_switch:
    pushfd
    pushad

    mov eax, [esp + 40]
    mov [eax + 0], edi
    mov [eax + 4], esi
    mov [eax + 8], ebp
    mov [eax + 12], ebx
    mov ecx, [esp + 32]
    mov [eax + 36], ecx
    lea ecx, [esp + 40]
    mov [eax + 28], ecx
    mov ecx, [esp + 36]
    mov [eax + 32], ecx

    mov edx, [esp + 44]
    mov edi, [edx + 0]
    mov esi, [edx + 4]
    mov ebp, [edx + 8]
    mov ebx, [edx + 12]

    push dword [edx + 36]
    popfd

    mov esp, [edx + 28]
    jmp dword [edx + 32]

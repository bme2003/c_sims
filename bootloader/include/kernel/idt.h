#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

#include "kernel/types.h"

struct interrupt_frame
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt_number;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

void idt_init(void);
void idt_enable_interrupts(void);
void isr_common_handler(const struct interrupt_frame *frame);

#endif

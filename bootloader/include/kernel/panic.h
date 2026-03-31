#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#include "kernel/idt.h"
#include "kernel/types.h"

void panic(const char *message);
void panic_exception(const struct interrupt_frame *frame);

#define KERNEL_ASSERT(condition, message) \
    do                                    \
    {                                     \
        if (!(condition))                 \
        {                                 \
            panic(message);               \
        }                                 \
    } while (0)

#endif

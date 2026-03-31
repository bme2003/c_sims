#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include "kernel/types.h"

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void io_wait(void);

#endif

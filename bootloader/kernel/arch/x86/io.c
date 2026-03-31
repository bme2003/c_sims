#include "kernel/io.h"

/* Read one byte from an x86 I/O port. */
uint8_t inb(uint16_t port)
{
    uint8_t value = 0;

    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/* Write one byte to an x86 I/O port. */
void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Insert a tiny delay after port I/O when older hardware expects it. */
void io_wait(void)
{
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

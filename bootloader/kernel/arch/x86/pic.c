#include "kernel/io.h"
#include "kernel/pic.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

/* Move PIC IRQ vectors out of the CPU exception range. */
void pic_remap(uint8_t master_offset, uint8_t slave_offset)
{
    uint8_t master_mask = inb(PIC1_DATA);
    uint8_t slave_mask = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, master_offset);
    io_wait();
    outb(PIC2_DATA, slave_offset);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

/* Acknowledge a handled IRQ so the PIC can deliver the next one. */
void pic_send_eoi(uint8_t irq_number)
{
    if (irq_number >= 8)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

/* Mask one IRQ line to temporarily block it from reaching the CPU. */
void pic_set_mask(uint8_t irq_number)
{
    uint16_t port = PIC1_DATA;
    uint8_t irq_line = irq_number;
    uint8_t value = 0;

    if (irq_number >= 8)
    {
        port = PIC2_DATA;
        irq_line = (uint8_t)(irq_number - 8);
    }

    value = inb(port) | (uint8_t)(1u << irq_line);
    outb(port, value);
}

/* Unmask one IRQ line so it can reach the CPU again. */
void pic_clear_mask(uint8_t irq_number)
{
    uint16_t port = PIC1_DATA;
    uint8_t irq_line = irq_number;
    uint8_t value = 0;

    if (irq_number >= 8)
    {
        port = PIC2_DATA;
        irq_line = (uint8_t)(irq_number - 8);
    }

    value = inb(port) & (uint8_t)~(1u << irq_line);
    outb(port, value);
}

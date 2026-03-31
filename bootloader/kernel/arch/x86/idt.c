#include "kernel/console.h"
#include "kernel/idt.h"
#include "kernel/keyboard.h"
#include "kernel/panic.h"
#include "kernel/pic.h"
#include "kernel/pit.h"

#define IDT_ENTRIES 256
#define KERNEL_CODE_SELECTOR 0x08
#define IDT_GATE_PRESENT 0x80
#define IDT_GATE_INTERRUPT_32 0x0E

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void isr0(void);
extern void isr13(void);
extern void isr14(void);
extern void irq0(void);
extern void irq1(void);

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_pointer idt_ptr;

/* Fill one IDT slot with the handler address and gate metadata. */
static void idt_set_gate(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[index].base_low = (uint16_t)(base & 0xFFFF);
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].flags = flags;
    idt[index].base_high = (uint16_t)((base >> 16) & 0xFFFF);
}

/* Load the finished IDT so the CPU starts using it for interrupts. */
static void idt_load(const struct idt_pointer *pointer)
{
    __asm__ volatile ("lidt %0" : : "m"(*pointer));
}

/* Build the early IDT with exception and hardware IRQ handlers. */
void idt_init(void)
{
    uint32_t index = 0;

    for (index = 0; index < IDT_ENTRIES; index++)
    {
        idt_set_gate((uint8_t)index, 0, 0, 0);
    }

    idt_set_gate(0, (uint32_t)isr0, KERNEL_CODE_SELECTOR, IDT_GATE_PRESENT | IDT_GATE_INTERRUPT_32);
    idt_set_gate(13, (uint32_t)isr13, KERNEL_CODE_SELECTOR, IDT_GATE_PRESENT | IDT_GATE_INTERRUPT_32);
    idt_set_gate(14, (uint32_t)isr14, KERNEL_CODE_SELECTOR, IDT_GATE_PRESENT | IDT_GATE_INTERRUPT_32);
    idt_set_gate(32, (uint32_t)irq0, KERNEL_CODE_SELECTOR, IDT_GATE_PRESENT | IDT_GATE_INTERRUPT_32);
    idt_set_gate(33, (uint32_t)irq1, KERNEL_CODE_SELECTOR, IDT_GATE_PRESENT | IDT_GATE_INTERRUPT_32);

    idt_ptr.limit = (uint16_t)(sizeof(idt) - 1);
    idt_ptr.base = (uint32_t)&idt[0];

    idt_load(&idt_ptr);
}

/* Enable hardware interrupts after the kernel has installed its handlers. */
void idt_enable_interrupts(void)
{
    __asm__ volatile ("sti");
}

/* Route interrupts to the right subsystem or panic on unexpected faults. */
void isr_common_handler(const struct interrupt_frame *frame)
{
    if (frame->interrupt_number == 32)
    {
        pit_handle_irq();
        pic_send_eoi(0);
        return;
    }

    if (frame->interrupt_number == 33)
    {
        keyboard_handle_irq();
        pic_send_eoi(1);
        return;
    }

    panic_exception(frame);
}

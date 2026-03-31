#include "kernel/console.h"
#include "kernel/panic.h"

/* Halt the CPU forever after a fatal kernel error. */
static void halt_forever(void)
{
    for (;;)
    {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}

/* Print a plain panic message and stop the kernel. */
void panic(const char *message)
{
    console_write("\nPANIC: ");
    console_write(message);
    console_write("\n");
    halt_forever();
}

/* Print key exception details from the saved interrupt frame and stop. */
void panic_exception(const struct interrupt_frame *frame)
{
    console_write("\nEXCEPTION: vector 0x");
    console_write_hex32(frame->interrupt_number);
    console_write(" error 0x");
    console_write_hex32(frame->error_code);
    console_write("\neip 0x");
    console_write_hex32(frame->eip);
    console_write(" cs 0x");
    console_write_hex32(frame->cs);
    console_write(" eflags 0x");
    console_write_hex32(frame->eflags);
    console_write("\n");
    halt_forever();
}

#include "kernel/console.h"
#include "kernel/io.h"
#include "kernel/pit.h"
#include "kernel/sched.h"

#define PIT_CHANNEL0_PORT 0x40
#define PIT_COMMAND_PORT 0x43
#define PIT_BASE_FREQUENCY 1193182u
#define PIT_COMMAND_CHANNEL0 0x00
#define PIT_COMMAND_ACCESS_LOHI 0x30
#define PIT_COMMAND_MODE3 0x06

static volatile uint32_t pit_ticks = 0;
static uint32_t heartbeat_frame = 0;

/* Program PIT channel 0 to generate periodic timer interrupts. */
void pit_init(uint32_t frequency_hz)
{
    uint32_t divisor = PIT_BASE_FREQUENCY / frequency_hz;

    if (frequency_hz == 0)
    {
        divisor = PIT_BASE_FREQUENCY / 100u;
    }

    outb(PIT_COMMAND_PORT, PIT_COMMAND_CHANNEL0 | PIT_COMMAND_ACCESS_LOHI | PIT_COMMAND_MODE3);
    outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

/* Expose the running timer tick count to the rest of the kernel. */
uint32_t pit_get_ticks(void)
{
    return pit_ticks;
}

/* Advance the tick counter and update the on-screen heartbeat spinner. */
void pit_handle_irq(void)
{
    static const char heartbeat_frames[] = "|/-\\";

    pit_ticks++;

    if ((pit_ticks % 100u) == 0)
    {
        console_putc_at(0, 79, heartbeat_frames[heartbeat_frame]);
        heartbeat_frame = (heartbeat_frame + 1u) % 4u;
    }

    scheduler_timer_tick();
}

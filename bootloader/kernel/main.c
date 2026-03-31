#include "kernel/boot.h"
#include "kernel/console.h"
#include "kernel/idt.h"
#include "kernel/keyboard.h"
#include "kernel/kmalloc.h"
#include "kernel/panic.h"
#include "kernel/paging.h"
#include "kernel/pic.h"
#include "kernel/pit.h"
#include "kernel/pmm.h"
#include "kernel/sched.h"

/* Keep the existing keyboard-driven exception self-test available inside tasks. */
static void handle_exception_test_request(const char *source_label)
{
    if (keyboard_take_exception_test_request())
    {
        console_write("[debug] triggering divide-by-zero exception from ");
        console_write(source_label);
        console_write("\n");
        __asm__ volatile (
            "xor %%edx, %%edx\n"
            "mov $1, %%eax\n"
            "xor %%ecx, %%ecx\n"
            "div %%ecx\n"
            :
            :
            : "eax", "ecx", "edx"
        );
    }
}

/* Wait for the PIT tick to advance while still honoring scheduler and debug events. */
static uint32_t wait_for_next_tick(uint32_t last_tick, const char *source_label)
{
    uint32_t current_tick = pit_get_ticks();

    while (current_tick == last_tick)
    {
        handle_exception_test_request(source_label);
        if (scheduler_should_yield())
        {
            scheduler_yield();
        }
        __asm__ volatile ("hlt");
        current_tick = pit_get_ticks();
    }

    return current_tick;
}

/* First demo task for cooperative scheduler bring-up. */
static void task_a_entry(void)
{
    uint32_t last_tick = pit_get_ticks();
    uint32_t emit_divider = 0;

    console_write("task A entered\n");

    for (;;)
    {
        last_tick = wait_for_next_tick(last_tick, "task A");
        emit_divider++;

        if (emit_divider >= 2u)
        {
            console_putc('A');
            emit_divider = 0;
        }
    }
}

/* Second demo task for cooperative scheduler bring-up. */
static void task_b_entry(void)
{
    uint32_t last_tick = pit_get_ticks();
    uint32_t emit_divider = 0;

    console_write("task B entered\n");

    for (;;)
    {
        last_tick = wait_for_next_tick(last_tick, "task B");
        emit_divider++;

        if (emit_divider >= 3u)
        {
            console_putc('B');
            emit_divider = 0;
        }
    }
}

/* Sleep forever when the scheduler has no normal runnable work. */
static void idle_task_entry(void)
{
    console_write("idle task entered\n");

    for (;;)
    {
        handle_exception_test_request("idle task");
        if (scheduler_should_yield())
        {
            scheduler_yield();
        }
        __asm__ volatile ("hlt");
    }
}

/* Idle the kernel while still servicing interrupt-driven debug actions. */
static void kernel_wait_loop(void)
{
    for (;;)
    {
        __asm__ volatile ("hlt");

        if (keyboard_take_exception_test_request())
        {
            console_write("[debug] triggering divide-by-zero exception\n");
            __asm__ volatile (
                "xor %%edx, %%edx\n"
                "mov $1, %%eax\n"
                "xor %%ecx, %%ecx\n"
                "div %%ecx\n"
                :
                :
                : "eax", "ecx", "edx"
            );
        }
    }
}

/* Verify that dynamic page mappings work outside the identity-mapped region. */
static void paging_mapping_smoke_test(void)
{
    volatile uint32_t *virtual_page = (volatile uint32_t *)0x00400000u;
    void *physical_page = pmm_alloc_page();

    if (physical_page == 0)
    {
        panic("paging mapping smoke test could not allocate physical page");
    }

    paging_map_page(0x00400000u, (uint32_t)physical_page, PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE);
    *virtual_page = 0xC0DEFACEu;

    if (*virtual_page != 0xC0DEFACEu)
    {
        panic("paging mapping smoke test failed");
    }

    console_write("paging mapping smoke test ok\n");
}

/* Bring up the early kernel subsystems and then drop into the idle loop. */
void kernel_main(const struct boot_info *boot)
{
    console_init();
    console_clear();

    console_write("kernel boot ok\n");
    console_write("boot drive: 0x");
    console_write_hex8(boot->boot_drive);
    console_write("\n");
    console_write("conventional memory (KB): 0x");
    console_write_hex32((unsigned int)boot->memory_kb);
    console_write("\n");
    console_write("kernel load address: 0x");
    console_write_hex32(boot->kernel_load_addr);
    console_write("\n");
    KERNEL_ASSERT(boot->kernel_load_addr != 0, "kernel load address was zero");

    idt_init();
    console_write("idt initialized\n");
    pic_remap(0x20, 0x28);
    console_write("pic remapped\n");
    pit_init(100);
    keyboard_init();
    pic_clear_mask(0);
    pic_clear_mask(1);
    console_write("pit and keyboard ready\n");
    idt_enable_interrupts();
    console_write("interrupts enabled\n");
    console_write("type normally to echo keyboard input\n");
    console_write("press Shift+1 (!) to trigger exception self-test\n");
    pmm_init(boot);
    console_write("pmm pages total: 0x");
    console_write_hex32(pmm_total_pages());
    console_write(" free: 0x");
    console_write_hex32(pmm_free_pages());
    console_write("\n");
    if (pmm_alloc_page() != 0)
    {
        console_write("pmm allocation smoke test ok\n");
    }
    else
    {
        panic("pmm allocation smoke test failed");
    }
    kmalloc_init();
    if (kmalloc(64u) != 0)
    {
        console_write("kmalloc smoke test ok\n");
    }
    else
    {
        panic("kmalloc smoke test failed");
    }
    paging_init();
    console_write("paging enabled\n");
    if (!paging_is_enabled())
    {
        panic("paging state did not update");
    }
    paging_mapping_smoke_test();
    task_init();
    {
        struct task *idle_task = task_create_kernel_named("idle", idle_task_entry);

        if (idle_task == (struct task *)0)
        {
            panic("idle task creation failed");
        }

        scheduler_set_idle_task(idle_task);
    }
    if (task_create_kernel_named("task_a", task_a_entry) == (struct task *)0)
    {
        panic("task_a creation failed");
    }
    if (task_create_kernel_named("task_b", task_b_entry) == (struct task *)0)
    {
        panic("task_b creation failed");
    }
    console_write("scheduler task creation smoke test ok count 0x");
    console_write_hex32(scheduler_task_count());
    console_write("\n");
    console_write("starting cooperative task scheduler\n");
    scheduler_start();

    kernel_wait_loop();
}

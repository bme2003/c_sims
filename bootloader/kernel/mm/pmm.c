#include "kernel/console.h"
#include "kernel/panic.h"
#include "kernel/pmm.h"

#define MAX_TRACKED_PAGES 256u

extern uint8_t __kernel_start;
extern uint8_t __kernel_end;

static uint8_t page_state[MAX_TRACKED_PAGES];
static uint32_t tracked_pages = 0;
static uint32_t free_pages = 0;

/* Round an address upward to the next page boundary. */
static uint32_t align_up(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1u) & ~(alignment - 1u);
}

/* Round an address downward to the previous page boundary. */
static uint32_t align_down(uint32_t value, uint32_t alignment)
{
    return value & ~(alignment - 1u);
}

/* Mark a physical page range as reserved in the early page bitmap. */
static void reserve_page_range(uint32_t start, uint32_t end)
{
    uint32_t first_page = align_down(start, PAGE_SIZE) / PAGE_SIZE;
    uint32_t last_page = align_up(end, PAGE_SIZE) / PAGE_SIZE;
    uint32_t page = 0;

    for (page = first_page; page < last_page; page++)
    {
        if (page < tracked_pages && page_state[page] == 0)
        {
            page_state[page] = 1;
            free_pages--;
        }
    }
}

/* Build the early physical page map from conventional memory information. */
void pmm_init(const struct boot_info *boot)
{
    uint32_t conventional_bytes = ((uint32_t)boot->memory_kb) * 1024u;
    uint32_t memory_limit = align_down(conventional_bytes, PAGE_SIZE);
    uint32_t page = 0;
    uint32_t kernel_start = (uint32_t)&__kernel_start;
    uint32_t kernel_end = (uint32_t)&__kernel_end;

    tracked_pages = memory_limit / PAGE_SIZE;
    if (tracked_pages > MAX_TRACKED_PAGES)
    {
        tracked_pages = MAX_TRACKED_PAGES;
    }

    for (page = 0; page < tracked_pages; page++)
    {
        page_state[page] = 0;
    }

    free_pages = tracked_pages;

    reserve_page_range(0x00000000u, 0x00010000u);
    reserve_page_range(0x00010000u, kernel_end);
    reserve_page_range(0x00080000u, 0x000A0000u);

    KERNEL_ASSERT(kernel_start >= 0x00010000u, "kernel start below expected load address");
    KERNEL_ASSERT(kernel_end > kernel_start, "kernel image layout invalid");
}

/* Return one free physical page frame from the early page bitmap. */
void *pmm_alloc_page(void)
{
    uint32_t page = 0;

    for (page = 0; page < tracked_pages; page++)
    {
        if (page_state[page] == 0)
        {
            page_state[page] = 1;
            free_pages--;
            return (void *)(page * PAGE_SIZE);
        }
    }

    return (void *)0;
}

/* Return one physical page frame back to the early page bitmap. */
void pmm_free_page(void *page)
{
    uint32_t page_index = ((uint32_t)page) / PAGE_SIZE;

    if (page_index >= tracked_pages)
    {
        return;
    }

    if (page_state[page_index] != 0)
    {
        page_state[page_index] = 0;
        free_pages++;
    }
}

/* Report how many physical pages the early PMM is tracking. */
uint32_t pmm_total_pages(void)
{
    return tracked_pages;
}

/* Report how many tracked physical pages are still free. */
uint32_t pmm_free_pages(void)
{
    return free_pages;
}

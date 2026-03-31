#include "kernel/kmalloc.h"
#include "kernel/pmm.h"

#define KMALLOC_ALIGNMENT 8u

static uint32_t heap_current = 0;
static uint32_t heap_remaining = 0;

/* Round allocations up so returned pointers keep a stable alignment. */
static uint32_t align_up(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1u) & ~(alignment - 1u);
}

/* Reset the early bump allocator before any dynamic allocations happen. */
void kmalloc_init(void)
{
    heap_current = 0;
    heap_remaining = 0;
}

/* Hand out small kernel allocations by carving space from PMM-backed pages. */
void *kmalloc(uint32_t size)
{
    uint32_t aligned_size = 0;
    void *page = 0;
    uint32_t allocation = 0;

    if (size == 0)
    {
        return (void *)0;
    }

    aligned_size = align_up(size, KMALLOC_ALIGNMENT);
    if (aligned_size > PAGE_SIZE)
    {
        return (void *)0;
    }

    if (heap_remaining < aligned_size)
    {
        page = pmm_alloc_page();
        if (page == 0)
        {
            return (void *)0;
        }

        heap_current = (uint32_t)page;
        heap_remaining = PAGE_SIZE;
    }

    allocation = heap_current;
    heap_current += aligned_size;
    heap_remaining -= aligned_size;
    return (void *)allocation;
}

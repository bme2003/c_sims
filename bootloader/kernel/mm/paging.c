#include "kernel/paging.h"
#include "kernel/panic.h"
#include "kernel/pmm.h"

#define IDENTITY_MAP_LIMIT 0x00100000u
#define PAGE_TABLE_ENTRIES 1024u
#define PAGE_DIRECTORY_INDEX(address) (((address) >> 22) & 0x3FFu)
#define PAGE_TABLE_INDEX(address) (((address) >> 12) & 0x3FFu)
#define PAGE_FRAME_MASK 0xFFFFF000u

static uint32_t *page_directory = (uint32_t *)0;
static uint32_t *first_page_table = (uint32_t *)0;
static int paging_enabled = 0;

/* Clear one page directory or page table page before filling entries. */
static void zero_page(uint32_t *page)
{
    uint32_t index = 0;

    for (index = 0; index < PAGE_TABLE_ENTRIES; index++)
    {
        page[index] = 0;
    }
}

/* Return the page table for a virtual address, creating one if requested. */
static uint32_t *get_page_table(uint32_t virtual_address, int create_if_missing)
{
    uint32_t directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    uint32_t directory_entry = page_directory[directory_index];
    uint32_t *page_table = (uint32_t *)0;

    if ((directory_entry & PAGING_FLAG_PRESENT) != 0)
    {
        return (uint32_t *)(directory_entry & PAGE_FRAME_MASK);
    }

    if (!create_if_missing)
    {
        return (uint32_t *)0;
    }

    page_table = (uint32_t *)pmm_alloc_page();
    KERNEL_ASSERT(page_table != 0, "failed to allocate page table");
    zero_page(page_table);
    page_directory[directory_index] = ((uint32_t)page_table) | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;
    return page_table;
}

/* Build the first page directory, identity-map low memory, and enable paging. */
void paging_init(void)
{
    uint32_t address = 0;

    page_directory = (uint32_t *)pmm_alloc_page();
    first_page_table = (uint32_t *)pmm_alloc_page();

    KERNEL_ASSERT(page_directory != 0, "failed to allocate page directory");
    KERNEL_ASSERT(first_page_table != 0, "failed to allocate first page table");

    zero_page(page_directory);
    zero_page(first_page_table);

    for (address = 0; address < IDENTITY_MAP_LIMIT; address += PAGE_SIZE)
    {
        first_page_table[address / PAGE_SIZE] = address | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;
    }

    page_directory[0] = ((uint32_t)first_page_table) | PAGING_FLAG_PRESENT | PAGING_FLAG_WRITABLE;

    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory) : "memory");
    __asm__ volatile (
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        :
        : "eax", "memory"
    );
    paging_enabled = 1;
}

/* Map one virtual page to one physical page with the requested flags. */
void paging_map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags)
{
    uint32_t *page_table = get_page_table(virtual_address, 1);
    uint32_t table_index = PAGE_TABLE_INDEX(virtual_address);
    uint32_t aligned_virtual = virtual_address & PAGE_FRAME_MASK;
    uint32_t aligned_physical = physical_address & PAGE_FRAME_MASK;

    page_table[table_index] = aligned_physical | (flags & 0xFFFu);

    if (paging_enabled)
    {
        __asm__ volatile ("invlpg (%0)" : : "r"(aligned_virtual) : "memory");
    }
}

/* Report whether the kernel has already switched paging on. */
int paging_is_enabled(void)
{
    return paging_enabled;
}

#ifndef KERNEL_PMM_H
#define KERNEL_PMM_H

#include "kernel/boot.h"
#include "kernel/types.h"

#define PAGE_SIZE 4096u

void pmm_init(const struct boot_info *boot);
void *pmm_alloc_page(void);
void pmm_free_page(void *page);
uint32_t pmm_total_pages(void);
uint32_t pmm_free_pages(void);

#endif

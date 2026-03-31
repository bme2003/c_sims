#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H

#include "kernel/types.h"

#define PAGING_FLAG_PRESENT 0x001u
#define PAGING_FLAG_WRITABLE 0x002u

void paging_init(void);
void paging_map_page(uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
int paging_is_enabled(void);

#endif

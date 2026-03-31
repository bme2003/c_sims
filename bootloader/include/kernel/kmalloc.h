#ifndef KERNEL_KMALLOC_H
#define KERNEL_KMALLOC_H

#include "kernel/types.h"

void kmalloc_init(void);
void *kmalloc(uint32_t size);

#endif

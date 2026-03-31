#ifndef KERNEL_BOOT_H
#define KERNEL_BOOT_H

#include "kernel/types.h"

struct boot_info
{
    uint8_t boot_drive;
    uint8_t reserved0;
    uint16_t memory_kb;
    uint32_t kernel_load_addr;
};

#endif

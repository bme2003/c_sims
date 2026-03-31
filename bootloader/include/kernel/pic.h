#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

#include "kernel/types.h"

void pic_remap(uint8_t master_offset, uint8_t slave_offset);
void pic_send_eoi(uint8_t irq_number);
void pic_set_mask(uint8_t irq_number);
void pic_clear_mask(uint8_t irq_number);

#endif

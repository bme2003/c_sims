#ifndef KERNEL_PIT_H
#define KERNEL_PIT_H

#include "kernel/types.h"

void pit_init(uint32_t frequency_hz);
uint32_t pit_get_ticks(void);
void pit_handle_irq(void);

#endif

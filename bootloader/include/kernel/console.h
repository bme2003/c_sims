#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H

#include "kernel/types.h"

void console_init(void);
void console_clear(void);
void console_putc(char character);
void console_putc_at(uint32_t row, uint32_t column, char character);
void console_write(const char *message);
void console_write_hex8(uint8_t value);
void console_write_hex32(uint32_t value);

#endif

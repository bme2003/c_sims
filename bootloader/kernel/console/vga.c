#include "kernel/console.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0F

static volatile unsigned short *const video_memory = (volatile unsigned short *)0xB8000;
static unsigned int cursor_row = 0;
static unsigned int cursor_column = 0;

/* Pack an ASCII character and its color into one VGA text-mode cell. */
static unsigned short make_vga_entry(char character, unsigned char color)
{
    return (unsigned short)((unsigned char)character | ((unsigned short)color << 8));
}

/* Write one character directly to a fixed VGA text-mode cell. */
static void write_cell(uint32_t row, uint32_t column, char character)
{
    video_memory[row * VGA_WIDTH + column] = make_vga_entry(character, VGA_COLOR);
}

/* Scroll the screen upward once the text cursor moves past the last row. */
static void console_scroll_if_needed(void)
{
    unsigned int row = 0;
    unsigned int column = 0;

    if (cursor_row < VGA_HEIGHT)
    {
        return;
    }

    for (row = 1; row < VGA_HEIGHT; row++)
    {
        for (column = 0; column < VGA_WIDTH; column++)
        {
            video_memory[(row - 1) * VGA_WIDTH + column] = video_memory[row * VGA_WIDTH + column];
        }
    }

    for (column = 0; column < VGA_WIDTH; column++)
    {
        write_cell(VGA_HEIGHT - 1, column, ' ');
    }

    cursor_row = VGA_HEIGHT - 1;
}

/* Reset the software cursor state used by the VGA console. */
void console_init(void)
{
    cursor_row = 0;
    cursor_column = 0;
}

/* Clear the whole VGA text buffer and reset the cursor to the top. */
void console_clear(void)
{
    unsigned int row = 0;
    unsigned int column = 0;

    for (row = 0; row < VGA_HEIGHT; row++)
    {
        for (column = 0; column < VGA_WIDTH; column++)
        {
            write_cell(row, column, ' ');
        }
    }

    cursor_row = 0;
    cursor_column = 0;
}

/* Write one character at the current cursor position, handling wrapping. */
void console_putc(char character)
{
    if (character == '\n')
    {
        cursor_row++;
        cursor_column = 0;
        console_scroll_if_needed();
        return;
    }

    write_cell(cursor_row, cursor_column, character);
    cursor_column++;

    if (cursor_column >= VGA_WIDTH)
    {
        cursor_column = 0;
        cursor_row++;
        console_scroll_if_needed();
    }
}

/* Write one character at a fixed screen position without moving the cursor. */
void console_putc_at(uint32_t row, uint32_t column, char character)
{
    if (row >= VGA_HEIGHT || column >= VGA_WIDTH)
    {
        return;
    }

    write_cell(row, column, character);
}

/* Write a null-terminated string through the console character routine. */
void console_write(const char *message)
{
    unsigned int index = 0;

    while (message[index] != '\0')
    {
        console_putc(message[index]);
        index++;
    }
}

/* Print one byte as two hexadecimal digits. */
void console_write_hex8(uint8_t value)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    console_putc(hex_digits[(value >> 4) & 0x0F]);
    console_putc(hex_digits[value & 0x0F]);
}

/* Print one 32-bit value as eight hexadecimal digits. */
void console_write_hex32(uint32_t value)
{
    int nibble = 0;

    for (nibble = 7; nibble >= 0; nibble--)
    {
        console_putc("0123456789ABCDEF"[(value >> (nibble * 4)) & 0x0F]);
    }
}

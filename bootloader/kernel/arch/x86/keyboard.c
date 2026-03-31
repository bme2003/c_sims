#include "kernel/console.h"
#include "kernel/io.h"
#include "kernel/keyboard.h"

#define LEFT_SHIFT_PRESS 0x2A
#define RIGHT_SHIFT_PRESS 0x36
#define LEFT_SHIFT_RELEASE 0xAA
#define RIGHT_SHIFT_RELEASE 0xB6

static volatile int exception_test_requested = 0;
static int shift_active = 0;

/* Translate a raw set-1 scancode into an ASCII character when possible. */
static char translate_scancode(uint8_t scancode)
{
    static const char unshifted_map[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,  '\\', 'z',
        'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,  '*', 0,  ' ', 0
    };
    static const char shifted_map[128] = {
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,  '|', 'Z',
        'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,  '*', 0,  ' ', 0
    };

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS)
    {
        shift_active = 1;
        return 0;
    }

    if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE)
    {
        shift_active = 0;
        return 0;
    }

    if (scancode & 0x80)
    {
        return 0;
    }

    if (scancode >= sizeof(unshifted_map))
    {
        return 0;
    }

    if (shift_active)
    {
        return shifted_map[scancode];
    }

    return unshifted_map[scancode];
}

/* Keep the early keyboard driver initialization hook in one place. */
void keyboard_init(void)
{
}

/* Read the keyboard controller and forward printable keys to the console. */
void keyboard_handle_irq(void)
{
    uint8_t scancode = inb(0x60);
    char character = translate_scancode(scancode);

    if (character != 0)
    {
        if (character == '!')
        {
            exception_test_requested = 1;
            console_write("\n[debug] exception self-test requested\n");
            return;
        }

        console_putc(character);
    }
}

/* Consume the one-shot exception test request from the keyboard path. */
int keyboard_take_exception_test_request(void)
{
    int requested = exception_test_requested;

    exception_test_requested = 0;
    return requested;
}

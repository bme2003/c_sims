#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

void keyboard_init(void);
void keyboard_handle_irq(void);
int keyboard_take_exception_test_request(void);

#endif

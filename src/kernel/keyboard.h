#ifndef _KEYBOARD_H
#define _KEYBOARD_H

void handle_keyboard_irq();
char scancode_to_ascii(int scancode);

#endif

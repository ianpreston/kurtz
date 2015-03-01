#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "libkernel.h"
#include "terminal.h"


void handle_keyboard_irq()
{
    uint8_t scancode = inb(0x60);

    // If the scancode is a pressed key, map it to ASCII and print it.
    if (scancode <= 0x57)
    {
        char ascii = scancode_to_ascii(scancode);
        if (ascii != '\0')
        {
            char message[2] = {ascii, '\0'};
            print(message);
        }
    }
}

char scancode_to_ascii(int scancode)
{
    // Big ugly map
    if (scancode >= 0x10 && scancode <= 0x1C)
    {
        char map[] = "qwertyuiop[]\n";
        return map[scancode - 0x10];
    }
    else if (scancode >= 0x1E && scancode <= 0x28)
    {
        char map[] = "asdfghjkl;'";
        return map[scancode - 0x1E];
    }
    else if (scancode >= 0x2C && scancode <= 0x35)
    {
        char map[] = "zxcvbnm,./";
        return map[scancode - 0x2C];
    }
    else if (scancode == 0x39)
    {
        return ' ';
    }

    return '\0';
}

#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "libkernel.h"
#include "terminal.h"


static bool shift = false;


void handle_keyboard_irq()
{
    uint8_t scancode = inb(0x60);

    // If the scancode is a pressed key, map it to ASCII and print it.
    if (scancode <= 0x57)
    {
        char ascii = scancode_to_ascii(scancode);
        if (ascii != '\0')
        {
            printf("%c", ascii);
        }
    }

    // Handle shift
    if (scancode == 0x2A || scancode == 0x36)
    {
        shift = true;
    }
    else if (scancode == 0xAA || scancode == 0xB6)
    {
        shift = false;
    }
}

char scancode_to_ascii(int scancode)
{
    int base = 0x02;
    char map_noshift[] = "1234567890-=\0\0qwertyuiop[]\n\0asdfghjkl;'\0\0\0zxcvbnm,./";
    char map_shift[] = "!@#$%^&*()_+\0\0QWERTYUIOP{}\n\0ASDFGHJKL;\"\0\0\0ZXCVBNM<>?";

    if (scancode >= base && scancode <= (base + sizeof(map_noshift)))
    {
        if (shift)
        {
            return map_shift[scancode - base];
        }
        else
        {
            return map_noshift[scancode - base];
        }
    }
    else if (scancode == 0x39)
    {
        return ' ';
    }

    return '\0';
}

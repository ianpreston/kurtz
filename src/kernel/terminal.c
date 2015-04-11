#include "terminal.h"
#include "libkernel.h"
#include <stdint.h>
#include <stdarg.h>
#define VIDEO_MEM 0xB8000


static int cursor_row;
static int cursor_col;
static char sexy;

/**
 * Public interface
 */

void init_terminal()
{
    cursor_row = 0;
    cursor_col = 0;
    disable_alert();
    clear_screen();
}

void printf(const char *format, ...)
{
    va_list argp;
    uint8_t arg_cur;

    va_start(argp, format);

    for (const char *c = format; *c != '\0'; c ++)
    {
        if (*c == '\a')
        {
            enable_alert();
            continue;
        }
        if (*c != '%')
        {
            put_char(*c);
            continue;
        }

        c ++;
        switch (*c)
        {
            case 'c':
                arg_cur = va_arg(argp, int);
                put_char(arg_cur);
                break;
            case 'x':
                arg_cur = va_arg(argp, int);
                printf("0x");
                print_hex(arg_cur);
                break;
            case '%':
                put_char('%');
                break;
        }
    }

    disable_alert();
    va_end(argp);
}

void clear_screen()
{
    for (int i = 0; i < 24; i++) {
        clear_row(i);
    }
}

/**
 * Internal interface
 */

void put_char(char c)
{
    if (c == '\n')
    {
        put_newline();
        return;
    }

    uint32_t offset = cursor_row * 80 + cursor_col;
    put_terminal_word(offset, c, sexy);

    cursor_col ++;
    if (cursor_col > 79)
    {
        put_newline();
    }
    set_textmode_cursor();
}

void put_newline()
{
    cursor_col = 0;
    if (cursor_row < 24)
    {
        cursor_row ++;
    } else {
        scroll_lines();
    }
    set_textmode_cursor();
}

void clear_row(int row)
{
    for (int i = 0; i < 80; i++)
    {
        put_terminal_word(row * 80 + i, 0, sexy);
    }
}

void put_terminal_word(uint32_t offset, char c, char s)
{
    uint8_t* addr = (uint8_t*)(VIDEO_MEM + (offset * 2));
    uint16_t word = (s << 8) | (c & 0xFF);
    memcpy((void*)addr, &word, sizeof word);
}

void scroll_lines()
{
    // Iterate over lines and copy the contents from the following line
    // into the current one
    for (int r=0; r<24; r++)
    {
        uint8_t* row = (uint8_t*)(VIDEO_MEM + (r * 160));
        uint8_t* next_row = (uint8_t*)(VIDEO_MEM + (r * 160) + 160);
        memcpy((void*)row, (void*)next_row, 160);
    }

    clear_row(24);
}

void set_textmode_cursor()
{
    uint16_t offset = cursor_row * 80 + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(offset & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((offset >> 8) & 0xFF));
}

/**
 * Helper/support functions
 */

void print_hex(int c)
{
    for (int i = 7; i >= 0; i--)
    {
        uint32_t divisor = pow(16, i);
        uint32_t mask = pow(16, i+1) - divisor;

        uint32_t bits = (c & mask) / divisor;
        switch (bits) {
            case 0 : put_char('0'); break;
            case 1 : put_char('1'); break;
            case 2 : put_char('2'); break;
            case 3 : put_char('3'); break;
            case 4 : put_char('4'); break;
            case 5 : put_char('5'); break;
            case 6 : put_char('6'); break;
            case 7 : put_char('7'); break;
            case 8 : put_char('8'); break;
            case 9 : put_char('9'); break;
            case 10: put_char('A'); break;
            case 11: put_char('B'); break;
            case 12: put_char('C'); break;
            case 13: put_char('D'); break;
            case 14: put_char('E'); break;
            case 15: put_char('F'); break;
            default: put_char('?'); break;
        };
    }
}

void enable_alert()
{
    sexy = 0x9;
}

void disable_alert()
{
    sexy = 0x2;
}

#include "terminal.h"
#include "libkernel.h"
#include <stdint.h>
#define VMEM 0xB8000


static int cursor_row;
static int cursor_col;
static char sexy;

void init_terminal()
{
    cursor_row = 0;
    cursor_col = 0;
    sexy = 0x02;
    clear_screen();
    print("Initalized text mode driver\n");
}

void put_terminal_word(uint32_t offset, char c, char s)
{
    uint8_t* addr = (uint8_t*)(VMEM + (offset * 2));
    uint16_t word = (s << 8) | (c & 0xFF);
    memcpy((void*)addr, &word, sizeof word);
}

void put_char(char c)
{
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

void scroll_lines()
{
    // Iterate over lines and copy the contents from the following line
    // into the current one
    for (int r=0; r<24; r++)
    {
        uint8_t* row = (uint8_t*)(VMEM + (r * 160));
        uint8_t* next_row = (uint8_t*)(VMEM + (r * 160) + 160);
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

void print(char *message)
{
    int i = 0;
    while (1) {
        char chr = message[i];
        if (chr == '\0') {
            return;
        }
        else if (chr == '\n') {
            put_newline();
        }
        else {
            put_char(chr);
        }
        i ++;
    }
}

void print_with_style(char *message, char style)
{
    char old_sexy = sexy;
    sexy = style;
    print(message);
    sexy = old_sexy;
}

void print_sys(char *message)
{
    print_with_style(message, 0x09);
}

void print_bad(char *message)
{
    print_with_style(message, 0x0C);
}

void print_hex(int c)
{
    print("0x");
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

void clear_row(int row)
{
    for (int i = 0; i < 80; i++)
    {
        put_terminal_word(row * 80 + i, 0, sexy);
    }
}

void clear_screen()
{
    for (int i = 0; i < 24; i++) {
        clear_row(i);
    }
}

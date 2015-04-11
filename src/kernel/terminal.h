#ifndef _TERMINAL_H
#define _TERMINAL_H
#include <stdint.h>

// Initializer - run before calling any other methods in this module.
void init_terminal();

// External interface
void printf(const char *format, ...);
void printf_sys(const char *format, ...);
void clear_screen();

// Internal interface
void put_char(char c);
void put_newline();
void put_terminal_word(uint32_t offset, char c, char s);
void clear_row(int row);
void scroll_lines();
void set_textmode_cursor();

// Helper/support functions
void enable_alert();
void disable_alert();
void print_hex(int c);
#endif

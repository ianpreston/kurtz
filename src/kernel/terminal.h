#ifndef _TERMINAL_H
#define _TERMINAL_H
#include <stdint.h>

// Initializer - run before calling any other methods in this module.
void init_terminal();

// High level interface
void print(char *message);
void print_sys(char *message);
void print_bad(char *message);
void print_hex(int c);
void clear_screen();

// Low level "internal" interface
void put_terminal_word(uint32_t offset, char c, char s);
void put_char(char c);
void put_newline();
void print_with_style(char *message, char style);
void clear_row(int row);
void scroll_lines();
void set_textmode_cursor();

#endif

#ifndef _LIBKERNEL_H
#define _LIBKERNEL_H
#include <stdint.h>
#include <stdbool.h>

void memset(void *start, uint8_t byte, int n);
void memcpy(void *dest, void *src, int n);

bool strcmp(char *left, char *right);

int pow(int base, int exp);

void outb(uint16_t port, uint8_t byte);
uint8_t inb(uint16_t port);

#endif

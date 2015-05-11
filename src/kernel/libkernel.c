#include "libkernel.h"


void memset(void *start, uint8_t byte, int n)
{
    uint8_t *addr = (uint8_t*)start;
    while (n)
    {
        *(addr) = byte;
        addr ++;
        n --;
    }
}

void memcpy(void *dest, void *src, int n)
{
    uint8_t *d = (uint8_t*)dest;
    uint8_t *s = (uint8_t*)src;
    while (n)
    {
        *d = *s;
        d ++;
        s ++;
        n --;
    }
}

bool strcmp(char *left, char *right)
{
    while (true)
    {
        if (*left != *right)
        {
            return false;
        }
        if (*left == '\0' && *right == '\0')
        {
            return true;
        }
        left++;
        right++;
    }
    return false;
}

void outb(uint16_t port, uint8_t byte)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (byte));
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

int pow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        result *= base;
        exp--;
    }
    return result;
}

#ifndef _KHEAP_H
#define _KHEAP_H
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    void *next;
    bool in_use;
} kheap_header_t;

void init_kheap();
void* kmalloc(uint32_t size);
void kfree(void *mallocd);

uint32_t more_kheap();

#endif

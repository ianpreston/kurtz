#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include <stdint.h>

typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t offset_high;
} __attribute__((packed)) idt_descriptor;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr;

typedef struct
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax,
             int_no, err_code, eip, cs,
             eflags, useresp, ss;
} registers_t;


void init_interrupts();

void init_idt();
void reset_pic();
void kernel_isr(registers_t r);

#endif

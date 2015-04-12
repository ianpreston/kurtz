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
             int_num, err_code, eip, cs,
             eflags, useresp, ss;
} __attribute__((packed)) registers_t;


// Public interface
void init_interrupts();

// Internal interface
void init_idt();
void reset_pic();
void irq_reset();
void kernel_isr(registers_t r);
void init_pit_timer();

#endif

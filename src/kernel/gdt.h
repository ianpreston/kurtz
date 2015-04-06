#ifndef _GDT_H
#define _GDT_H
#include <stdint.h>
#include <stdbool.h>


typedef struct {
    unsigned int limit_low: 16;
    unsigned int base_low: 24;

    // Access Byte
    unsigned int accessed: 1;
    unsigned int rw: 1;
    unsigned int conform: 1;
    unsigned int ex: 1;
    unsigned int seg: 1;
    unsigned int priv: 2;
    unsigned int present: 1;

    // Limit/Flags Byte
    unsigned int limit_high: 4;
    unsigned int avail: 1;
    unsigned int l: 1;
    unsigned int size: 1;
    unsigned int granularity: 1;

    unsigned int base_high: 8;
} __attribute__((__packed__)) gdt_entry_t;

typedef struct {
    uint64_t null;

    gdt_entry_t kcode;
    gdt_entry_t kdata;
    gdt_entry_t ucode;
    gdt_entry_t udata;
    gdt_entry_t ktss;
} __attribute__((__packed__)) gdt_table_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((__packed__)) gdt_ptr_t;

typedef struct {
    uint32_t prev;
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap, iomap;
} __attribute__((__packed__)) tss_entry_t;


void init_gdt();

void make_gdt_segment(gdt_entry_t *entry, bool code, bool kernel);
void make_gdt_tss(gdt_entry_t *entry, tss_entry_t* tss);

#endif

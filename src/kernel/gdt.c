#include "gdt.h"
#include "libkernel.h"


static gdt_table_t gdt;
static gdt_ptr_t gdt_ptr;
static tss_entry_t kernel_tss;

extern void flush_gdt(uint32_t addr);
extern void flush_tss();

void make_gdt_segment(gdt_entry_t *entry, bool code, bool kernel)
{
    *entry = (gdt_entry_t){
        .base_low = 0x0,
        .base_high = 0x0,
        .limit_low = 0xFFFF,
        .limit_high = 0xF,

        .present = 1,
        .priv = kernel ? 0x0 : 0x3,
        .seg = 1,
        .ex = code,
        .conform = 0,
        .rw = 1,
        .accessed = 0,

        .avail = 0,
        .l = 0,
        .size = 1,
        .granularity = 1
    };
}

void make_gdt_tss(gdt_entry_t *entry, tss_entry_t* tss)
{
    uint32_t tss_base = (uint32_t)tss;
    uint32_t tss_limit = tss_base + sizeof(*tss);

    *entry = (gdt_entry_t){
        .base_low = tss_base & 0xFFFFFF,
        .base_high = (tss_base & 0xFF000000) >> 24,
        .limit_low = tss_limit & 0xFFFF,
        .limit_high = (tss_limit & 0xF0000) >> 16,

        .present = 1,
        .priv = 0x3,
        .seg = 0,
        .ex = 1,
        .conform = 0,
        .rw = 0,
        .accessed = 1, // TODO - Has different meaning for TSS GDT entries. Rename?

        .avail = 0,
        .l = 0,
        .size = 0,
        .granularity = 0,
    };
}

static void init_kernel_tss()
{
    memset(&kernel_tss, 0, sizeof(kernel_tss));
    kernel_tss.esp0 = 0x90000;
    kernel_tss.ss0 = 0x10;

    kernel_tss.cs = 0xB;
    kernel_tss.ds = 0x13;
    kernel_tss.es = 0x13;
    kernel_tss.fs = 0x13;
    kernel_tss.gs = 0x13;
    kernel_tss.ss = 0x13;
}

void init_gdt()
{
    memset(&gdt, 0, sizeof(gdt));

    // Make code and data segment entries in the GDT for both privilege levels.
    make_gdt_segment(&gdt.kcode, true, true);
    make_gdt_segment(&gdt.ucode, true, false);
    make_gdt_segment(&gdt.kdata, false, true);
    make_gdt_segment(&gdt.udata, false, false);

    // Make task state segment entry in the GDT.
    make_gdt_tss(&gdt.ktss, &kernel_tss);

    // Initialize the task state segment itself
    init_kernel_tss();

    // Install the GDT
    gdt_ptr.base = &gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;
    flush_gdt((uint32_t)&gdt_ptr);
    flush_tss();
}

#include <stdint.h>
#include "libkernel.h"
#include "gdt.h"
#include "terminal.h"
#include "interrupt.h"
#include "paging.h"
#include "kheap.h"
#include "proc.h"
#include "ramfs.h"
#include "initrd.h"


typedef struct
{
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t mod_cmdline;
} __attribute__((packed)) modinfo_t;

typedef struct
{
   uint32_t flags;
   uint32_t mem_lower;
   uint32_t mem_upper;
   uint32_t boot_device;
   uint32_t cmdline;
   uint32_t mods_count;
   uint32_t mods_addr;
   uint32_t num;
   uint32_t size;
   uint32_t addr;
   uint32_t shndx;
   uint32_t mmap_length;
   uint32_t mmap_addr;
   uint32_t drives_length;
   uint32_t drives_addr;
   uint32_t config_table;
   uint32_t boot_loader_name;
   uint32_t apm_table;
   uint32_t vbe_control_info;
   uint32_t vbe_mode_info;
   uint32_t vbe_mode;
   uint32_t vbe_interface_seg;
   uint32_t vbe_interface_off;
   uint32_t vbe_interface_len;
}  __attribute__((packed)) bootinfo_t;


int main(uint32_t magic, bootinfo_t *header)
{
    init_gdt();

    init_paging();
    init_kheap();

    init_terminal();

    init_proc();
    init_ramfs();

    init_interrupts();

    if (magic != 0x2BADB002)
    {
        printf("\aInvalid multiboot magic number");
        return 1;
    }

    if (header->mods_count != (uint32_t)1)
    {
        printf("\aInvalid number of multiboot modules");
        return 1;
    }

    printf("\aBooted Kernel Kurtz v0.0.0!\n");
    printf("Multiboot flags=%x\n", header->flags);

    printf("Loading initial ramdisk\n");
    modinfo_t *initrd_mod = (modinfo_t*)header->mods_addr;
    load_initrd((void*)initrd_mod->mod_start);

    printf("Spawning processes\n");
    proc_t *first_proc = proc_spawn();
    proc_t *second_proc = proc_spawn();

    printf("first_proc=%x\n", first_proc->pid);
    printf("    cs_base=%x cs_p_addr=%x\n", first_proc->cs_base, first_proc->cs_p_addr);
    printf("second_proc=%x\n", second_proc->pid);
    printf("    cs_base=%x cs_p_addr=%x\n", second_proc->cs_base, second_proc->cs_p_addr);

    load_helloworld_binary(first_proc);
    load_helloworld_binary(second_proc);

    printf("\aDropping to user mode!\n\n");
    drop_to_usermode();

    return 0;
}

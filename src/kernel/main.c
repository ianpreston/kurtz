#include <stdint.h>
#include "libkernel.h"
#include "gdt.h"
#include "terminal.h"
#include "interrupt.h"
#include "paging.h"
#include "kheap.h"
#include "proc.h"


void init_pit_timer()
{
    uint32_t divisor = 1193180 / 5; // 5Hz
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

int main()
{
    init_gdt();

    init_paging();
    init_kheap();

    init_terminal();

    init_interrupts();
    init_pit_timer();

    init_proc();

    print_sys("Booted Kernel Kurtz v0.0.0!\n\n");

    print("kmalloc() test...\n");
    void *ptr0 = kmalloc(31);
    print_hex((uint32_t)ptr0);
    print(" ");

    void *ptr1 = kmalloc(8);
    print_hex((uint32_t)ptr1);
    print(" ");

    kfree(ptr1);
    kfree(ptr0);

    void *ptr2 = kmalloc(128);
    print_hex((uint32_t)ptr2);
    print("\n");
    kfree(ptr2);

    print("Spawning processes\n");
    proc_t *first_proc = spawn_proc();
    proc_t *second_proc = spawn_proc();

    load_helloworld_binary(first_proc);
    load_helloworld_binary(second_proc);

    print_sys("Dropping to user mode!\n");
    drop_to_usermode();

    return 0;
}

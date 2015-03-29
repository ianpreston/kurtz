#include <stdint.h>
#include "libkernel.h"
#include "gdt.h"
#include "terminal.h"
#include "interrupt.h"
#include "paging.h"
#include "kheap.h"


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

    print_sys("Booted Kernel Kurtz v0.0.0!\n\n");

    print("Interrupt test...\n");
    asm volatile ("int $0xF1");

    print("kmalloc() test...\n");
    void *ptr0 = kmalloc(31);
    print_hex(ptr0);
    print(" ");

    void *ptr1 = kmalloc(8);
    print_hex(ptr1);
    print(" ");

    kfree(ptr1);
    kfree(ptr0);

    void *ptr2 = kmalloc(128);
    print_hex(ptr2);
    print("\n");
    kfree(ptr2);

    return 0;
}

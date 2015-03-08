#include <stdint.h>
#include "libkernel.h"
#include "terminal.h"
#include "interrupt.h"
#include "paging.h"


void init_pit_timer()
{
    uint32_t divisor = 1193180 / 5; // 5Hz
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

int main()
{
    init_terminal();

    init_paging();

    init_interrupts();
    init_pit_timer();

    print_sys("Booted Kernel Kurtz v0.0.0!\n\n");

    print("Interrupt test...\n");
    asm volatile ("int $0xF1");

    print("Virtual memory test...\n");
    vmem_alloc(0xA1BEEF);
    *(uint8_t*)(0xA1BEEF) = ':';
    *(uint8_t*)(0xA1BEF0) = 'D';
    put_char(*(uint8_t*)(0xA1BEEF));
    put_char(*(uint8_t*)(0xA1BEF0));
    vmem_free(0xA1BEEF);
    print("\n");

    return 0;
}

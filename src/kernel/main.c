#include <stdint.h>
#include "libkernel.h"
#include "terminal.h"
#include "interrupt.h"


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
    init_interrupts();

    init_pit_timer();

    print_sys("Boot-up seems to have been successful...\n");
    print_sys("Welcome to Kernel Kurtz v0.0.0\n");
    asm volatile ("int $0xF1");

    return 0;
}

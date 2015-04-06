#include "syscall.h"
#include "interrupt.h"
#include "terminal.h"
#include "proc.h"


void fire_syscall(uint8_t int_no, registers_t regs)
{
    print_sys("fire_syscall: yield\n");
    yield_from(regs.eip, regs.useresp);
}

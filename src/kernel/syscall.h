#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <stdint.h>
#include "interrupt.h"


void fire_syscall(uint8_t int_no, registers_t regs);

#endif

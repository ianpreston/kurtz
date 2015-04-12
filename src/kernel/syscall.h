#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <stdint.h>
#include "interrupt.h"


// Public interface
void syscall_fire(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

// Internal interface
void sys_write(uint32_t string_addr);
void sys_exit(uint32_t code);

#endif

#include "syscall.h"
#include "interrupt.h"
#include "terminal.h"
#include "proc.h"


void syscall_fire(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx)
{
    switch (eax)
    {
        case 0x1:
            sys_exit(ebx);
            break;
        case 0x4:
            sys_write(ebx);
            break;
        default:
            printf("\aUnknown syscall: %x\n", eax);
            break;
    }
}

void sys_write(uint32_t string_addr)
{
    printf("%s", (char*)string_addr);
}

void sys_exit(uint32_t code)
{
    printf("Process exits with code %x\n", code);
    proc_exit();
}

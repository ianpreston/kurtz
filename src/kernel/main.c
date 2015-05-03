#include <stdint.h>
#include "libkernel.h"
#include "gdt.h"
#include "terminal.h"
#include "interrupt.h"
#include "paging.h"
#include "kheap.h"
#include "proc.h"


int main()
{
    init_gdt();

    init_paging();
    init_kheap();

    init_terminal();

    init_proc();

    init_interrupts();

    printf("\aBooted Kernel Kurtz v0.0.0!\n");

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

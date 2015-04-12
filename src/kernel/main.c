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

    load_helloworld_binary(first_proc);
    load_helloworld_binary(second_proc);

    printf("\aDropping to user mode!\n\n");
    drop_to_usermode();

    return 0;
}
